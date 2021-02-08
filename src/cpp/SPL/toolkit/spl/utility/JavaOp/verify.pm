#
# Copyright 2021 IBM Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

 use File::Temp  ;

 ##
 ## Compile time processing for Java operators.
 ##

sub verify($)
{
 my ($model) = @_;
 # Describe the operator invocation as a set of properties
 # in a temp file encoded with UTF-8.
 my $outDir = $model->getContext()->getOutputDirectory();



 my $unlinkFile = 1;
 $unlinkFile = 0 if ($ENV{STREAMS_SAVE_JAVA_PARAM_FILE});
 my $tmp = File::Temp->new( TEMPLATE => 'javaXXXXX',
                        DIR => $outDir,
                        UNLINK => $unlinkFile,
                        SUFFIX => '.txt');
  binmode($tmp, ":utf8");

  open $SPL::CodeGen::OLDSTDOUT, ">&", "STDOUT"
    or SPLDie(1, SPL::Msg::CANT_DUP_STDOUT());

  # Add verbose mode
  print $tmp 'verboseMode=' . ($model->getContext()->isVerboseModeOn() ? 'true' : 'false')  . "\n";

  # Add the generated flag
  my $generated = $model->getParameterByName("generated");
  $generated = $generated ? $generated->getValueAt(0)->getSPLExpression() : 'false';
  print $tmp 'generated=' . $generated . "\n";

  my $operclass = $model->getParameterByName("className")->getValueAt(0)->getSPLExpression();
  print $tmp 'className=' . $operclass . "\n";

  # Add the classLibrary values.
  {
    my $clparam=$model->getParameterByName("classLibrary");
    for (my $pi = 0; $pi < $clparam->getNumberOfValues(); $pi++) {
      my $clexpr = $clparam->getValueAt($pi);
      my $cte = undef;
      $cte = SPL::CodeGen::compileTimeExpression($model, $clexpr) if ($clexpr->hasSPLExpressionTree());
      my $isComplex;
      if (defined($cte)) {
          print $tmp 'classLibrary.' . $pi . '=' . $cte . "\n";
          $isComplex = "false";
      } else {
          print $tmp 'classLibrary.' . $pi . '=' . $clexpr->getSPLExpression() . "\n";
          $isComplex = $clexpr->hasFunctions() || $clexpr->hasSideEffects() ? "true" : "false";
      }
      # Pass through if the SPL expression (Attribute free) has
      # functions or other complexities we won't resolve in Java.
      print $tmp 'classLibrary.' . $pi . '.complex=' . $isComplex . "\n";
    }
  }

  # All the parameters (names only).
  #foreach my $param (@{$model->getParameters()}) {
  #    print $tmp "parameter.". $param->getName()  . "\n";
  #}

  # Description of the input ports
  print $tmp 'inputPortCount=' . $model->getNumberOfInputPorts() . "\n";

  foreach my $inport (@{$model->getInputPorts()}) {
    # Description of the port
    print $tmp 'inputPort.' . $inport->getIndex() . '=' .
         $inport->getNumberOfAttributes() . "\n";
    # Description of the port's attributes
    for (my $ai = 0; $ai < $inport->getNumberOfAttributes(); $ai++) {
      my $attr = $inport->getAttributeAt($ai);

      print $tmp 'inputPort.' . $inport->getIndex() . '.' .
           $ai . '=' . $attr->getSPLType() . '-' . $attr->getName() . "\n";
    }
    if ($inport->hasWindow()) {
       my $param = 'window.' . $inport->getIndex();
       my $win = $inport->getWindow();
       my $wpi = 0;
       print $tmp $param . '.' . $wpi++ . '=' . $win->getWindowType() . "\n";
       print $tmp $param . '.' . $wpi++ . '=' . $win->isPartitioned() . "\n";
       my $evict = $win->getEvictionPolicyType();
       print $tmp $param . '.' . $wpi++ . '=' . $evict . "\n";
       print $tmp $param . '.' . $wpi++ . '=' . $win->getEvictionPolicyAttribute()->getSPLExpression() . "\n" if ($evict == $SPL::Operator::Instance::Window::DELTA);
       # eviction policy size hard coded for compile time
       print $tmp $param . '.' . $wpi++ . '=' . "1\n" unless ($evict == $SPL::Operator::Instance::Window::PUNCT);

      if ($win->isSliding()) {
         my $trigger = $win->getTriggerPolicyType();
         print $tmp $param . '.' . $wpi++ . '=' . $trigger . "\n";
         print $tmp $param . '.' . $wpi++ . '=' . $win->getTriggerPolicyAttribute()->getSPLExpression() ."\n" if ($trigger == $SPL::Operator::Instance::Window::DELTA);
         # trigger policy size hard coded for compile time.
         print $tmp $param . '.' . $wpi++ . "=0\n";
      }

      if ($win->hasPartitionEvictionPolicy()) {
         my $partitionEvictPolicy = $win->getPartitionEvictionPolicyKind();
           print $tmp $param . '.' . $wpi++ . '=' . $partitionEvictPolicy . "\n";

           # partition policy size hard coded for compile time
           print $tmp $param . '.' . $wpi++ . "=1\n";
         }
    }
  }

  # Description of the output ports
  print $tmp 'outputPortCount=' . $model->getNumberOfOutputPorts() . "\n";

  foreach my $outport (@{$model->getOutputPorts()}) {
    # Description of the port
    print $tmp 'outputPort.' . $outport->getIndex() . '=' .
         $outport->getNumberOfAttributes() . "\n";
    # Description of the port's attributes
    for (my $ai = 0; $ai < $outport->getNumberOfAttributes(); $ai++) {
      my $attr = $outport->getAttributeAt($ai);

      print $tmp 'outputPort.' . $outport->getIndex() . '.' .
           $ai . '=' . $attr->getSPLType() . '-' . $attr->getName() . "\n";
    }
  }

  close($tmp);

  my $javaHome = $ENV{'JAVA_HOME'};
  my $streamsInstall = $ENV{'STREAMS_INSTALL'};
  my $streamsJvmCache = $ENV{'STREAMS_JVM_CACHE'};
  $streamsJvmCache = lc $streamsJvmCache;

  my $isIBMJava = 0;
  if ($javaHome =~ m/.*ibm.*/ || $javaHome =~ m/^$streamsInstall/) {
    $isIBMJava = 1;
  }

  my $useJvmCache = 1;
  if ($isIBMJava == 0 || $streamsJvmCache == "false") {
    $useJvmCache = 0;
  }

  my @jcga = ($javaHome . '/bin/java', '-cp');
  push(@jcga, $streamsInstall . '/lib/com.ibm.streams.operator.samples.jar');
  push(@jcga, '-Dlog4j.configuration=com/ibm/streams/operator/internal/logtrace/log4j/log4j.properties');
  push(@jcga, '-Xquickstart') if ($isIBMJava);
  push(@jcga, '-Xshareclasses:name=com.ibm.streams.spl.sc_%u,nonfatal,silent') if ($useJvmCache);
  push(@jcga, 'com.ibm.streams.operator.internal.compile.OperatorCompileTime');

  # First argument is the name of the file with the properties
  push(@jcga, $tmp->filename);
  # Then information to find the operatorInstanceModel XML file
  push(@jcga, $outDir);
  push(@jcga, $model->getContext()->getClass());

  if ($generated eq 'true') {
      SPL::CodeGen::printlnVerbose(SPL::Msg::STDTK_JAVAOP_GENERATING_TEMPLATE_CODE($operclass));
  }

  # Ensure output from compile time checks do not corrupt the
  # C++ template code.
  # dependency-scan: ignore the following line
  my $output = `@jcga`;
  SPL::CodeGen::printlnVerbose($output) if ($output ne '');
  if ($? == -1) {
    SPL::CodeGen::errorln(SPL::Msg::STDTK_JAVAOP_GENERATING_TEMPLATE_CODE_RUN_FAILED, $model->getContext()->getSourceLocation());
  }
  else {
   my $rc = $? >> 8;
    if ($rc == 1) {
      SPL::CodeGen::errorln(SPL::Msg::STDTK_JAVAOP_GENERATING_TEMPLATE_CODE_FAILED, $model->getContext()->getSourceLocation());
   }
   if ($rc == 2) {
      SPL::CodeGen::errorln(SPL::Msg::STDTK_JAVAOP_OPERATOR_INVOCATION_ERROR(
           $model->getContext()->getKind()),
           $model->getContext()->getSourceLocation());
   }

   open STDOUT, ">&", $SPL::CodeGen::OLDSTDOUT
     or SPLDie(1, SPL::Msg::CANT_RESTORE_STDOUT());
   close $SPL::CodeGen::OLDSTDOUT;
 }

}

1;
