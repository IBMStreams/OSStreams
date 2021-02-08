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

#!/usr/bin/env perl

use strict;
use Getopt::Long qw(:config no_ignore_case bundling);
use XML::SAX;
use base qw(XML::SAX::Base);
use Data::Dumper;

our $handler = 0;

package Choice;
1;

package Sequence;
1;

package SyntacticPredicate;
1;
#******************************************************
package Element;

sub new
{
   my ($class, $kind) = @_;
#   print "Processing the start of a $kind\n";
   my $self = { _previousHandler => $main::handler, _kind => $kind };
   $main::handler = $self;
   bless $self, $class;
   return $self;
}

sub end_element
{
   my ($self, $element) = @_;

   if ($element->{'LocalName'} eq $self->{_kind}) {
       $main::handler = $self->{_previousHandler};
#       print "Processing the end of a $self->{_kind}\n";
   } else {
       die("$self->{_kind}: Unexpected element type ".$element->{'LocalName'});
   }
}

sub characters
{
   my ($self, $text) = @_;
#   print "$self->{_kind} needs to override method 'characters'\n";
}

1;
#******************************************************
package Not;
our @ISA = qw(Element);

sub new
{
   my ($class, $element) = @_;
   my $self = $class->SUPER::new('not');
   my $attributes = $element->{Attributes};
   $self->{_repetition} = $attributes->{'{}repetition'}->{Value};
   bless $self, $class;
   return $self;
}

sub start_element
{
   my ($self, $element) = @_;
   if ($element->{'LocalName'} eq 'sequence') {
      push(@{$self->{_rule}}, new Sequence($element));
   } elsif ($element->{'LocalName'} eq 'choice') {
      push(@{$self->{_rule}}, new Choice($element));
   } else {
      die("Production::Unexpected element type $element->{'LocalName'}\n");
   }
}

1;
#******************************************************
package Dot;
our @ISA = qw(Element);

sub new
{
   my ($class, $element) = @_;
   my $self = $class->SUPER::new('dot');
   my $attributes = $element->{Attributes};
   $self->{_repetition} = $attributes->{'{}repetition'}->{Value};
   bless $self, $class;
   return $self;
}

1;
#******************************************************
package SpecialTerminal;
our @ISA = qw(Element);

sub new
{
   my ($class, $element) = @_;
   my $self = $class->SUPER::new('specialTerminal');
   $self->{_text} = "";
   bless $self, $class;
   return $self;
}

sub characters
{
   my ($self, $text) = @_;
   $self->{_text} = $text;
}

1;
#******************************************************
package Terminal;
our @ISA = qw(Element);

sub new
{
   my ($class, $element) = @_;
   my $self = $class->SUPER::new('terminal');
   $self->{_text} = "";
   bless $self, $class;
   return $self;
}

sub characters
{
   my ($self, $text) = @_;
   $self->{_text} = $text;
}

1;

#******************************************************
package Enumeration;
our @ISA = qw(Element);

sub new
{
   my ($class, $element) = @_;
   my $self = $class->SUPER::new('enumeration');
   $self->{_text} = "";
   bless $self, $class;
   return $self;
}

sub characters
{
   my ($self, $text) = @_;
   $self->{_text} = $text;
}

1;

#******************************************************
package TerminalRange;
our @ISA = qw(Element);

sub new
{
   my ($class, $element) = @_;
   my $self = $class->SUPER::new('terminalRange');
   $self->{_range} = ();
   bless $self, $class;
   return $self;
}

sub start_element
{
   my ($self, $element) = @_;
   if ($element->{'LocalName'} eq 'terminal') {
      push(@{$self->{_range}}, new Terminal($element));
   }
}

1;
#******************************************************
package Argument;
our @ISA = qw(Element);

sub new
{
   my ($class, $element) = @_;
   my $self = $class->SUPER::new('args');
   $self->{_text} = "";
   bless $self, $class;
   return $self;
}

sub characters
{
   my ($self, $text) = @_;
   $self->{_text} = $text;
}

1;
#******************************************************
package NonTerminal;
our @ISA = qw(Element);

sub new
{
   my ($class, $element) = @_;
   my $self = $class->SUPER::new('nonTerminal');
   my $attributes = $element->{Attributes};
   $self->{_name} = $attributes->{'{}name'}->{Value};
   $self->{_repetition} = $attributes->{'{}repetition'}->{Value};
   $self->{_args} = ();
   bless $self, $class;
   return $self;
}

sub start_element
{
   my ($self, $element) = @_;
   if ($element->{'LocalName'} eq 'args') {
      push(@{$self->{_args}}, new Argument($element));
   }
}

1;

#******************************************************
package Choice;
our @ISA = qw(Element);

sub new
{
   my ($class, $element) = @_;
   my $self = $class->SUPER::new('choice');
   my $attributes = $element->{Attributes};
   $self->{_repetition} = $attributes->{'{}repetition'}->{Value};
   $self->{_rule} = ();
   bless $self, $class;
   return $self;
}

sub start_element
{
   my ($self, $element) = @_;
   if ($element->{'LocalName'} eq 'sequence') {
      push(@{$self->{_rule}}, new Sequence($element));
   } elsif ($element->{'LocalName'} eq 'choice') {
      push(@{$self->{_rule}}, new Choice($element));
   } elsif ($element->{'LocalName'} eq 'nonTerminal') {
   	  push(@{$self->{_rule}}, new NonTerminal($element));
   } else {
      die("Choice::Unexpected element type $element->{'LocalName'}\n");
   }
}

1;
#******************************************************
package Sequence;
our @ISA = qw(Element);

sub new
{
   my ($class, $element) = @_;
   my $self = $class->SUPER::new('sequence');
   $self->{_rule} = ();
   bless $self, $class;
   return $self;
}

sub start_element
{
   my ($self, $element) = @_;
   if ($element->{'LocalName'} eq 'terminal') {
      push(@{$self->{_rule}}, new Terminal($element));
   } elsif ($element->{'LocalName'} eq 'terminalRange') {
      push(@{$self->{_rule}}, new TerminalRange($element));
   } elsif ($element->{'LocalName'} eq 'specialTerminal') {
      push(@{$self->{_rule}}, new SpecialTerminal($element));
   } elsif ($element->{'LocalName'} eq 'nonTerminal') {
      push(@{$self->{_rule}}, new NonTerminal($element));
   } elsif ($element->{'LocalName'} eq 'choice') {
      push(@{$self->{_rule}}, new Choice($element));
   } elsif ($element->{'LocalName'} eq 'syntacticPredicate') {
      push(@{$self->{_rule}}, new SyntacticPredicate($element));
   } elsif ($element->{'LocalName'} eq 'dot') {
      push(@{$self->{_rule}}, new Dot($element));
   } elsif ($element->{'LocalName'} eq 'not') {
      push(@{$self->{_rule}}, new Not($element));
   } else {
      die("Sequence::Unexpected element type $element->{'LocalName'}\n");
   }
}

1;
#******************************************************
package SyntacticPredicate;
our @ISA = qw(Element);

sub new
{
   my ($class, $element) = @_;
   my $self = $class->SUPER::new('syntacticPredicate');
   $self->{_rule} = ();
   bless $self, $class;
   return $self;
}

sub start_element
{
   my ($self, $element) = @_;
   if ($element->{'LocalName'} eq 'sequence') {
      push(@{$self->{_rule}}, new Sequence($element));
   } else {
      die("Sequence::Unexpected element type $element->{'LocalName'}\n");
   }
}

1;
#******************************************************
package Production;
our @ISA = qw(Element);

sub new
{
   my ($class, $element) = @_;
   my $self = $class->SUPER::new('production');
   $self->{_rule} = ();
   my $attributes = $element->{Attributes};
   $self->{_name} = $attributes->{'{}name'}->{Value};
   $self->{_gen} = $attributes->{'{}gen'}->{Value};
   bless $self, $class;
   return $self;
}

sub start_element
{
   my ($self, $element) = @_;
   if ($element->{'LocalName'} eq 'sequence') {
      push(@{$self->{_rule}}, new Sequence($element));
   } elsif ($element->{'LocalName'} eq 'choice') {
      push(@{$self->{_rule}}, new Choice($element));
   } elsif ($element->{'LocalName'} eq 'terminal') {
      push(@{$self->{_rule}}, new Terminal($element));
   } elsif ($element->{'LocalName'} eq 'specialTerminal') {
      push(@{$self->{_rule}}, new SpecialTerminal($element));
   } elsif ($element->{'LocalName'} eq 'enumeration') {
      push(@{$self->{_rule}}, new Enumeration($element));
   } else {
      die("Production::Unexpected element type $element->{'LocalName'}\n");
   }
}

1;
#******************************************************
package Fragment;
our @ISA = qw(Element);

sub new
{
   my ($class, $element) = @_;
   my $self = $class->SUPER::new('fragment');
   $self->{_rule} = ();
   my $attributes = $element->{Attributes};
   $self->{_name} = $attributes->{'{}name'}->{Value};
   $self->{_gen} = $attributes->{'{}gen'}->{Value};
   bless $self, $class;
   return $self;
}

sub start_element
{
   my ($self, $element) = @_;
   if ($element->{'LocalName'} eq 'sequence') {
      push(@{$self->{_rule}}, new Sequence($element));
   } elsif ($element->{'LocalName'} eq 'choice') {
      push(@{$self->{_rule}}, new Choice($element));
   } else {
      die("Production::Unexpected element type $element->{'LocalName'}\n");
   }
}

1;
#******************************************************
package Grammar;
our @ISA = qw(Element);

sub new
{
   my ($class, $element) = @_;
   my ($class, $element) = @_;
   my $self = $class->SUPER::new('grammar');
   $self->{_productions} = ();
   $self->{_fragments} = ();
   bless $self, $class;
   return $self;
}

sub start_element
{
   my ($self, $element) = @_;
   if ($element->{'LocalName'} eq 'production') {
      push(@{$self->{_productions}}, new Production($element));
   } elsif ($element->{'LocalName'} eq 'fragment') {
      push(@{$self->{_fragments}}, new Fragment($element));
   } else {
      die("Grammar::Unexpected element type ".$element->{'LocalName'});
   }
}

1;
#******************************************************
package GrammarParser;

sub new
{
   my ($class) = @_;
   my $self = { };
   $self->{ _parser } = XML::SAX::ParserFactory->parser(Handler=>$self);
   bless $self, $class;
   return $self;
}

sub start_document
{
   my ($self) = @_;
#   print "Processing the start of document\n";
}

sub start_element
{
   my ($self, $element) = @_;
   if ($element->{'LocalName'} eq 'grammar') {
      $self->{_grammar} = new Grammar($element);
   } else {
      $handler->start_element($element);
   }
}

sub characters
{
   my ($self, $data) = @_;
   $handler->characters($data->{Data});
}

sub end_element
{
   my ($self, $element) = @_;
   $main::handler->end_element($element);
}

sub end_document
{
   my ($self) = @_;
#   print "Processing the end of document\n";
}

sub parse
{
   my ($self, $file) = @_;
   $self->{_parser}->parse_uri($file);
}
1;
