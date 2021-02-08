/*
 * Copyright 2021 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.ibm.streams.manifest;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.file.*;
import java.nio.file.attribute.BasicFileAttributes;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import java.util.concurrent.*;
import java.util.jar.*;
import java.util.stream.Collectors;
import java.util.stream.IntStream;
import lombok.var;
import org.apache.commons.cli.*;

class Main {

  private static class FileCollector extends SimpleFileVisitor<Path> {

    private Path installPath;
    private String prefix;
    private Map<String, String> content;
    private PathMatcher matcher;

    FileCollector(Path installPath, String prefix, Map<String, String> content) {
      this.installPath = installPath;
      this.prefix = prefix;
      this.content = content;
      this.matcher = FileSystems.getDefault().getPathMatcher("glob:" + prefix + "/*.jar");
    }

    @Override
    public FileVisitResult visitFile(Path file, BasicFileAttributes attrs) {
      /*
       * Skip files that don't match.
       */
      if (!matcher.matches(file.subpath(installPath.getNameCount(), file.getNameCount()))) {
        return FileVisitResult.CONTINUE;
      }
      /*
       * Skip symbolic links.
       */
      if (Files.isSymbolicLink(file)) {
        return FileVisitResult.CONTINUE;
      }
      /*
       * Collect the path prefix.
       */
      content.put(file.getFileName().toString(), prefix);
      return FileVisitResult.CONTINUE;
    }

    @Override
    public FileVisitResult visitFileFailed(Path file, IOException exc) {
      return FileVisitResult.CONTINUE;
    }
  }

  private static class ManifestUpdater extends SimpleFileVisitor<Path> {

    private Path installPath;
    private String prefix;
    private String dotPrefix;
    private Map<String, String> content;
    private PathMatcher matcher;

    ManifestUpdater(Path installPath, String prefix, Map<String, String> content) {
      this.installPath = installPath;
      this.prefix = prefix;
      this.dotPrefix =
          IntStream.range(0, Paths.get(prefix).getNameCount())
              .mapToObj(e -> "..")
              .collect(Collectors.joining("/"));
      this.content = content;
      this.matcher = FileSystems.getDefault().getPathMatcher("glob:" + prefix + "/*.jar");
    }

    private String convert(String entry) {
      /*
       * Check if the entry is already fully formed.
       */
      var entryPath = Paths.get(entry);
      if (Files.exists(installPath.resolve(this.prefix).resolve(entryPath))) {
        return entry;
      }
      /*
       * Check if the entry is valid.
       */
      if (!this.content.containsKey(entry)) {
        throw new RuntimeException("Invalid Class-Path entry: " + entry);
      }
      /*
       * Compute the relative path and make sure it is valid.
       */
      var relativePath = Paths.get(this.dotPrefix, this.content.get(entry), entry);
      var fullPath = installPath.resolve(this.prefix).resolve(relativePath);
      if (!Files.exists(fullPath)) {
        throw new RuntimeException("Invalid Class-Path entry: " + entry);
      }
      return relativePath.toString();
    }

    @Override
    public FileVisitResult visitFile(Path file, BasicFileAttributes attrs) throws IOException {
      /*
       * Check if the file match the pattern.
       */
      if (!matcher.matches(file.subpath(installPath.getNameCount(), file.getNameCount()))) {
        return FileVisitResult.CONTINUE;
      }
      /*
       * Skip symbolic links.
       */
      if (Files.isSymbolicLink(file)) {
        return FileVisitResult.CONTINUE;
      }
      /*
       * Load the manifest.
       */
      var zipfs = FileSystems.newFileSystem(file, null);
      var mf = zipfs.getPath("/META-INF/MANIFEST.MF");
      var manifest = new Manifest(Files.newInputStream(mf));
      var attributes = manifest.getMainAttributes();
      var classPath = attributes.getValue("Class-Path");
      if (classPath == null) {
        return FileVisitResult.CONTINUE;
      }
      /*
       * Compute the new classpath.
       */
      var result =
          Arrays.stream(classPath.split(" "))
              .map(this::convert)
              .sorted()
              .collect(Collectors.joining(" "));
      manifest.getMainAttributes().put(new Attributes.Name("Class-Path"), result);
      /*
       * Alter the classpath.
       */
      var is = new JarInputStream(Files.newInputStream(file));
      var tf = File.createTempFile("st_", ".jar");
      var os = new JarOutputStream(new FileOutputStream(tf), manifest);
      /*
       * Scan and write the entries;
       */
      int len;
      var buffer = new byte[1024];
      JarEntry entry;
      while ((entry = is.getNextJarEntry()) != null) {
        os.putNextEntry(entry);
        while ((len = is.read(buffer)) > 0) {
          os.write(buffer, 0, len);
        }
      }
      is.close();
      os.close();
      /*
       * Delete the original file and move the new file.
       */
      Files.delete(file);
      Files.move(tf.toPath(), file);
      return FileVisitResult.CONTINUE;
    }

    @Override
    public FileVisitResult visitFileFailed(Path file, IOException exc) {
      return FileVisitResult.CONTINUE;
    }
  }

  private static void waitFutureNoException(Future future) {
    try {
      future.get();
    } catch (InterruptedException | ExecutionException e) {
      e.printStackTrace();
    }
  }

  private static void processInstallPath(String path)
      throws ExecutionException, InterruptedException {
    var insPath = Paths.get(path);
    var libFiles = new ConcurrentHashMap<String, String>();
    /*
     * Classify the libraries.
     */
    var exec = Executors.newWorkStealingPool();
    List<Callable<Path>> collectors =
        Arrays.asList(
            () -> Files.walkFileTree(insPath, new FileCollector(insPath, "lib", libFiles)),
            () -> Files.walkFileTree(insPath, new FileCollector(insPath, "ext/lib", libFiles)),
            () ->
                Files.walkFileTree(
                    insPath, new FileCollector(insPath, "system/impl/lib", libFiles)));
    exec.invokeAll(collectors).forEach(Main::waitFutureNoException);
    exec.shutdown();
    /*
     * Alter the manifests.
     */
    exec = Executors.newWorkStealingPool();
    List<Callable<Path>> updaters =
        Arrays.asList(
            () -> Files.walkFileTree(insPath, new ManifestUpdater(insPath, "lib", libFiles)),
            () ->
                Files.walkFileTree(
                    insPath, new ManifestUpdater(insPath, "system/impl/lib", libFiles)));
    exec.invokeAll(updaters).forEach(Main::waitFutureNoException);
    exec.shutdown();
  }

  public static void main(String[] args) {
    /*
     * Define the command line options.
     */
    var options = new Options();
    var installPath = new Option("i", "install", true, "Streams install path");
    installPath.setRequired(true);
    options.addOption(installPath);
    /*
     * Define the command line parser.
     */
    var parser = new GnuParser();
    var formatter = new HelpFormatter();
    try {
      var cmd = parser.parse(options, args);
      processInstallPath(cmd.getOptionValue("install"));
    } catch (ParseException e) {
      e.printStackTrace();
      formatter.printHelp("streams.manifest", options);
      System.exit(1);
    } catch (ExecutionException | InterruptedException e) {
      e.printStackTrace();
      System.exit(1);
    }
  }
}
