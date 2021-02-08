#! /usr/bin/env python3.1

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

# fixlib module
# Match and replace stext in files in a library using regular expressions.
# A 'library' is any set of files with given extensions nested in a directory.
# To run, you can import this and call run(), or you can run fl.py.
# Author: Thomas Sharp, sharpt@us.ibm.com
# Version: 0.3

import sys            # For sys.argv
import os             # For os.walk
import fnmatch        # For fnmatch
import re             # For regular expression functions

PAT_KEY = "Pat:";
REP_KEY = "Rep:";
pat_list = [];
cpat_list = [];
rep_list = [];
ext_list = [];
wild_list = [];
flag_list = [];
flag_set = False
comma_pat = re.compile(',');
backup = True;
verbose = True;
file_total = 0;
total = 0;

# function to show help (specify -h)
def show_help():
  print("Syntax: [python] fixlib.py [-d dir-name] [-h] [-s fixlib.fls] [-e ext1[,ext2]...] [-w wildcard1[,wildcard2]...][-b]");
  print("where the square brackets indicate optional elements.");
  print("  -s fixlib.fls specifies the name of a script that contains");
  print("     match and replace expressions. fixlib.fls is the default.");
  print("  -d dir-name is the root directory of your library.");
  print("     If it is not specified, then the current directory is searched.");
  print("  -h displays syntax help.");
  print("  -b Avoids backing up the original files (adding .bak to their names).");
  print("  -q suppresses messages.");
  print("  -c removes backup files; use this option with -d only.");
  print("  -r restores backup files; use this option with -d only.");
  print("  -e ext1[,ext2]... specifies one or more file extensions to match.");
  print("     The default extensions are html,htm. These are case sensitive.");
  print("  -w wildcard1[,wildcard2]... specifies wildcard filenames to match.");
  print("     By default, only extensions are used.");

# function to scan the input arguments and return a value for a given flag
def get_flag_value(flag):
   i = 1;
   for v in sys.argv:
     if v == flag:
       return sys.argv[i];
     else:
       i += 1;
   return None;

# function to get the root directory of the library
def get_root():
  v = get_flag_value('-d');
  if v == None:
    return "."; # default root
  else:
    return v;

# function to get the name of the fixlib script
def get_script():
  v = get_flag_value('-s');
  if v == None:
    return "fixlib.fls"; # default script
  else:
    return v;

# function to get the wildcards
def get_wildcards():
  v = get_flag_value('-w');
  if v == None:
    return None;
  else:
    return comma_pat.split(v);


# function to get the extensions
def get_exts():
  v = get_flag_value('-e');
  if (v == None) and ('-w' not in sys.argv):
    return ['html','htm']; # default extensions
  elif v != None:
    return comma_pat.split(v);
  else:
    return [];

# function to compile a pattern
def compile_pat (pat):
  re_flags = 0;
  set_flag = False;
  while 1:
    if pat.startswith('(?m)'):
      re_flags |= re.MULTILINE;
      pat = pat[4:];
    elif pat.startswith('(?F)'):
      set_flag = True;
      pat = pat[4:];
    elif pat.startswith('(?i)'):
      re_flags |= re.IGNORECASE;
      pat = pat[4:];
    elif pat.startswith('(?im)') or pat.startswith('(?mi)'):
      re_flags |= (re.IGNORECASE | re.MULTILINE);
      pat = pat[5:];
    else:
      break;
  flag_list.append (set_flag);
  return re.compile(pat,re_flags);

# function to read a fixlib script
def read_fls(fls):
  fh = open(fls, 'r');
  for line in fh.readlines():
    if line.startswith(PAT_KEY):
      pat_list.append(line.strip()[len(PAT_KEY):].strip());
      cpat_list.append(compile_pat(pat_list[len(pat_list) - 1]));
    elif line.startswith(REP_KEY):
      rep_list.append(line.strip()[len(REP_KEY):].strip());
  fh.close();

# function to loop through the patterns and apply the replacements
def fix_file(content):
  i = 0;
  file_total = 0;
  flag_set = False;
  for p in cpat_list:
    if flag_set and flag_list[i]:
      if verbose:
        print "\tSkipping pattern:", pat_list[i];
      i += 1;
      continue; # don't apply this pattern
    ctuple = p.subn(rep_list[i], content);
    if ctuple[1] > 0:
      content = ctuple[0];
      file_total += int(ctuple[1]);
      if flag_list[i]:
        flag_set = True;
        if verbose:
          print "\tSet flag";
      if verbose:
        if ctuple[1] == 1:
          print "\tFound", ctuple[1], "match of", pat_list[i];
          print "\t\treplacement:", rep_list[i];
        else:
          print "\tFound", ctuple[1], "matches of", pat_list[i];
          print "\t\treplacement:", rep_list[i];
    i += 1;
  return [content,file_total];

# function to process a file
def process(d, f): # directory and file names
  if verbose:
    print(f, ": processing . . .");
  inname = os.path.join(d, f);
  outname = os.path.join(d, f + ".out");
  bakname = os.path.join(d, f + ".bak");
  fh = open(inname, 'r');
  content = fh.read();
  fh.close();
  # perform the matches and replacements:
  content_total = fix_file(content);
  content = content_total[0];
  file_total = content_total[1];
  if file_total > 0:
    # write the output file
    fh = open(outname, 'w');
    fh.write(content);
    fh.close();
    # Rename the original and new files
    if os.path.exists(bakname):
      os.remove(bakname);
    if backup:
      os.rename(inname, bakname);
    else:
      os.remove(inname);
    os.rename(outname, inname);
  return file_total;

# Main program
def run():
  if '-h' in sys.argv:
    show_help();
    sys.exit();
  verbose = True;
  if '-v' in sys.argv:
    verbose = False;

  root = get_root();
  # Do we want to only remove the backup files?
  if '-c' in sys.argv:
    # Walk the directory from the root, filter out the *.bak files
    # by their extensions, and delete them:
    for r, dirs, files in os.walk(root):
      for f in files:
        if fnmatch.fnmatch(f, '*.bak'):
          bakname = os.path.join(r, f);
          if os.path.exists(bakname):
            if verbose:
              print("Removing:", bakname);
            os.remove(bakname);
    sys.exit();
  # Do we want to restore the backups?
  elif '-r' in sys.argv:
    for r, dirs, files in os.walk(root):
      for f in files:
        if fnmatch.fnmatch(f, '*.bak'):
          bakname = os.path.join(r, f);
          origname = bakname[:len(bakname) - 4];
          if verbose:
            print("Restoring:", origname, "from", bakname);
          if os.path.exists(origname):
            os.remove(origname);
          os.rename(bakname, origname);
    sys.exit();

  # Otherwise, continue interpreting the command line options
  script = get_script();
  wild_list = get_wildcards();
  ext_list = get_exts();
  if '-b' in sys.argv:
    backup = False;

  read_fls(script);	# populate pat_list and rep_list

  lpl = len(pat_list);
  if lpl == 0:
    print("Please specify patterns in", script);
    sys.exit();
  lrl = len(rep_list);
  if lpl != lrl:
    print("Specify an equal number of pattern and replacement expressions in", script);
    sys.exit();

  # Walk the directory from the root, filter out the files
  # by their extensions, and process them:
  total = 0;
  for r, dirs, files in os.walk(root):
    if verbose:
      print("Found files in", r, " . . .");
    # for d in dirs: print("dir: ", d);
    for f in files:
      if '-w' in sys.argv:
        for w in wild_list:
          if fnmatch.fnmatch(f, w):
            file_total = process(r, f);
            total += file_total;
      for e in ext_list:
        if fnmatch.fnmatch(f, '*.' + e):
          file_total = process(r, f);
          total += file_total;
  if verbose:
    print("Total changes:", total);

# end of fixlib.py
