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

package com.ibm.streams.admin.internal.api;

import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.admin.internal.core.InternalUtilities;
import com.ibm.streams.management.persistence.PersistencePath;
import com.ibm.streams.messages.StreamsAdminMessagesKey.Key;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.file.Files;
import java.nio.file.LinkOption;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;
import java.nio.file.attribute.GroupPrincipal;
import java.nio.file.attribute.PosixFileAttributeView;
import java.nio.file.attribute.PosixFileAttributes;
import java.nio.file.attribute.PosixFilePermission;
import java.nio.file.attribute.UserPrincipal;
import java.security.MessageDigest;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Properties;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.xml.bind.DatatypeConverter;

/**
 * StreamsInstall represents a physical Streams installation. It can be used to retrieve information
 * related to the installation such as the install owner, and version information.
 *
 * <p>Use the factory methods to retrieve instances of StreamsInstall. For example, to get a
 * StreamsInstall object that represents the install pointed to by the STREAMS_INSTALL environment
 * variable:
 *
 * <pre> StreamsInstall streamsInstall =
 * StreamsInstall.getDefaultStreamsInstall(); </pre>
 *
 * <p>To retrieve a StreamsInstall object for a specific directory:
 *
 * <pre> String path =
 * "/myhome/myinstalldir"; StreamsInstall streamsInstall =
 * StreamsInstall.getStreamsInstall(path); </pre>
 */
public class StreamsInstall {

  public static final String PRODUCT_FILE = ".product";

  public static final String ISO_READINESS_DIR = "/swidtag";
  public static final String ISO_READINESS_EXT = ".swidtag";

  // install packages
  public static final String JAVA_PACKAGE = "streamsjava";
  public static final String JCE_PACKAGE = "streamsjce";
  public static final String CORE_PACKAGE = "streamscore";
  public static final String SWS_PACKAGE = "streamssws";
  public static final String DEVEL_PACKAGE = "streamsdevel";

  // install tgz filenames
  public static final String JAVA_PKG_TGZ = "streamsjava.tgz";
  public static final String JCE_PKG_TGZ = "streamsjce.tgz";
  public static final String CORE_PKG_TGZ = "streamscore.tgz";
  public static final String DEVEL_PKG_TGZ = "streamsdevel.tgz";
  public static final String ZK_PKG_TGZ = "streamsmanagedzk.tgz";
  public static final String TREE_PKG_TGZ = "streamstree.tgz";
  public static final String SWS_PKG_TGZ = "streamssws.tgz";
  public static final String SWS_PKG_MARKER = ".streamssws";

  // NOTE: If this version pattern is updated also need to update the version pattern in
  // Other/INST/InstallProject/src/com/ibm/streams/inst/common/CommonUtils.java.
  // last .<digits>  is optional for "hotfix"
  private static final String VERSION_PATTERN =
      "([0-9])+[\\.]([0-9])+[\\.]([0-9])+[\\.]([0-9])+[\\.]?([0-9])*+";

  private static String defPath = null;

  private String installHome;
  private Properties productInfo;
  private String osStr;
  private String arch;
  private String version;
  private String name;
  private String build;
  private String editionName;
  private String editionshortname;
  private String hotFix;
  private File prodFile;
  private String osVersion;
  private String osPatch;

  //    private Map<String, PropertyDefinition> propertyDefinitionsMap;

  private static final String localPolicyChecksum = "C76F08DDFAD8E6EF7AFC87F31BE88A06";
  private static final String usExportPolicyChecksum = "2E42C295422A92E04A547CD5BCBB9713";

  /**
   * Constructor - private
   *
   * @param path
   * @throws StreamsException
   */
  private StreamsInstall(String path) throws StreamsException {
    this.installHome = path;

    try {
      File installDir = new File(path);
      prodFile = new File(installDir, PRODUCT_FILE);
      FileInputStream in = new FileInputStream(prodFile);
      productInfo = new Properties();
      productInfo.load(in);
      in.close();

      osStr = productInfo.getProperty(PersistencePath.OS, "");
      arch = productInfo.getProperty(PersistencePath.ARCHITECTURE, "");
      version = productInfo.getProperty(PersistencePath.VERSION, "");
      name = productInfo.getProperty(PersistencePath.PRODUCT_NAME, "");
      build = productInfo.getProperty(PersistencePath.BUILD, "");
      editionName = productInfo.getProperty(PersistencePath.EDITION_NAME, "");
      editionshortname = productInfo.getProperty(PersistencePath.EDITION_SHORT_NAME, "");
      hotFix = productInfo.getProperty(PersistencePath.HOTFIX, "");
      osVersion = productInfo.getProperty(PersistencePath.OSVERSION, "");
      osPatch = productInfo.getProperty(PersistencePath.OSPATCH, "");
    } catch (IOException ioe) {
      Trace.logError(ioe.getMessage(), ioe);
      throw new StreamsException(ioe);
    }
  }

  /**
   * Retrieve the Streams install based on the environment variable setting for the current user.
   * This factory method retrieves the environment setting for STREAMS_INSTALL, and returns a
   * StreamsInstall object based on the environment setting.
   *
   * @return A StreamsInstall object representing the Streams install. Returns null if the
   *     environment is not set.
   * @exception NotInstalledException Thwown if the install directory does not exist.
   * @exception NotReadableException Thrown if the current user does not have read access to the
   *     install directory.
   * @exception NotExecutableException Thrown if the current user does not have execute access to
   *     the install directory.
   * @exception NotProperlyInstalledException Thrown if the install directory does not appear to be
   *     properly installed.
   */
  public static StreamsInstall getDefaultStreamsInstall()
      throws NotInstalledException, NotReadableException, NotExecutableException,
          NotProperlyInstalledException, StreamsException {
    String path;
    if (defPath != null) {
      path = defPath;
    } else {
      path = StreamsUtilities.getStreamsInstall();
    }

    Trace.logTrace("Install path: " + path);

    if (path != null) {
      return getStreamsInstall(path);
    } else {
      return null;
    }
  }

  /**
   * Retrieve a StreamsInstall based on the specified path.
   *
   * @param path Specifies the path to the Streams install.
   * @return A StreamsInstall object representing the Streams install. Returns null if the
   *     environment is not set.
   * @exception NotInstalledException Thwown if the install directory does not exist.
   * @exception NotReadableException Thrown if the current user does not have read access to the
   *     install directory.
   * @exception NotExecutableException Thrown if the current user does not have execute access to
   *     the install directory.
   * @exception NotProperlyInstalledException Thrown if the install directory does not appear to be
   *     properly installed.
   */
  public static StreamsInstall getStreamsInstall(String path)
      throws NotInstalledException, NotReadableException, NotExecutableException,
          NotProperlyInstalledException, StreamsException {
    // make sure the directory exists, the user can read and execute, and the .product file is there
    // (installed
    // correctly)
    File prodDir = new File(path);
    if (!prodDir.exists()) {
      // throw exception for product not installed
      NotInstalledException nie = new NotInstalledException(Key.ERR_PROD_DIR_EXISTS, path);
      if (Trace.isEnabled(Level.DEBUG)) {
        Trace.logDebug("Product directory does not exist " + path);
      }
      throw nie;
    }
    if (!prodDir.canRead()) {
      // throw exception for not able to read product dir
      NotReadableException nre = new NotReadableException(Key.ERR_PATH_NOT_READ, path);
      if (Trace.isEnabled(Level.DEBUG)) {
        Trace.logDebug("Failed read access to product directory " + path);
      }
      throw nre;
    }
    if (!prodDir.canExecute()) {
      // throw exception for not able to execute product dir
      NotExecutableException nee = new NotExecutableException(Key.ERR_PATH_NOT_EXEC, path);
      if (Trace.isEnabled(Level.DEBUG)) {
        Trace.logDebug("Failed execute access to product directory " + path);
      }
      throw nee;
    }
    File prodFile = new File(prodDir, PRODUCT_FILE);
    if (!prodFile.exists()) {
      // throw exception for product not installed properly
      NotProperlyInstalledException npie =
          new NotProperlyInstalledException(Key.ERR_PATH_NOT_INSTALLED, path);
      if (Trace.isEnabled(Level.DEBUG)) {
        Trace.logDebug("Product file not found " + prodFile.getPath());
      }
      throw npie;
    }

    try {
      // call getCanonicalPath() so symbolic links will get resolved
      return new StreamsInstall(prodDir.getCanonicalPath());
    } catch (IOException ioe) {
      Trace.logError(ioe.getMessage(), ioe);
      throw new StreamsException(ioe);
    }
  }

  /**
   * Returns true if the string represents a properly formatted version number
   *
   * @param version - String
   * @return boolean
   */
  public static boolean isVersion(String version) {
    Pattern pattern = Pattern.compile(VERSION_PATTERN);
    Matcher matcher = pattern.matcher(version);
    return matcher.matches();
  }

  /** Set this StreamsInstall as the default install path. */
  public void setAsDefault() {
    defPath = installHome;
  }

  /**
   * Check to see if the current user is the owner of this install.
   *
   * @return True if the current user is the owner of the install, false if not.
   */
  public boolean isOwner() throws StreamsException {
    String user = System.getProperty("user.name");
    return isOwner(user);
  }

  /**
   * Check to see if the specified user is the owner of this install.
   *
   * @return True if the user is the owner of the install, false if not.
   */
  public boolean isOwner(String user) throws StreamsException {
    if (user != null) {
      return user.equals(getOwner());
    } else {
      return false;
    }
  }

  /**
   * Convenience method to verify that the current user is properly authorized to the install
   * directory. This method uses STREAMS_INSTALL environment variable to determine the install path.
   * It verifies that the install directory exists, and the current user has read and execute access
   * to the directory, as well as the .product file exists in the install directory.
   *
   * @return True if the install directory exists, the current user has read and execute access to
   *     the install directory, and the .product file exists in the install directory.
   */
  public static boolean isAuthorized() {
    String path = StreamsUtilities.getStreamsInstall();

    return isAuthorized(path);
  }

  /**
   * Convenience method to verify that the current user is properly authorized to the specified
   * install directory. It verifies that the install directory exists, and the current user has read
   * and execute access to the directory, as well as the .product file exists in the install
   * directory.
   *
   * @return True if the install directory exists, the current user has read and execute access to
   *     the install directory, and the .product file exists in the install directory.
   */
  public static boolean isAuthorized(String path) {
    if (path == null) {
      return false;
    }
    File prodDir = new File(path);
    if (!prodDir.exists()) {
      return false;
    }
    if (!prodDir.canRead()) {
      return false;
    }
    if (!prodDir.canExecute()) {
      return false;
    }
    File prodFile = new File(prodDir, PRODUCT_FILE);
    if (!prodFile.exists()) {
      return false;
    }

    return true;
  }

  /**
   * Retrieve the owner of this Steams install.
   *
   * @return The owner of the install. Returns null if it is not able to determine the owner of the
   *     directory.
   */
  public String getOwner() throws StreamsException {
    try {
      if (installHome != null) {
        String[] cmd = {"stat", "-c", "%U", installHome};
        ProcessBuilder pb = new ProcessBuilder(cmd);
        pb.redirectErrorStream(true);
        Process process = pb.start();
        int exitValue = -1;
        boolean completed = false;
        while (!completed) {
          try {
            exitValue = process.waitFor();
            completed = true;
          } catch (InterruptedException ie) {
            // Trace.logError("Failed to execute stat on " + installHome, ie);
            // throw new StreamsException(ie);
          }
        }
        BufferedReader rdr = new BufferedReader(new InputStreamReader(process.getInputStream()));
        String line;
        if (exitValue == 0) {
          line = rdr.readLine();
          InternalUtilities.cleanupProcess(process);
          return line;
        } else {
          if (Trace.isEnabled(Level.DEBUG)) {
            Trace.logDebug("Failed to stat " + installHome + " " + exitValue);
          }
          while ((line = rdr.readLine()) != null) {
            if (Trace.isEnabled(Level.DEBUG)) {
              Trace.logDebug(line);
            }
          }
          InternalUtilities.cleanupProcess(process);
          return null;
        }
      } else {
        if (Trace.isEnabled(Level.DEBUG)) {
          Trace.logDebug("installHome is null, unable to determine install owner");
        }
        return null;
      }
    } catch (IOException ioe) {
      Trace.logError(ioe.getMessage(), ioe);
      throw new StreamsException(ioe);
    }
  }

  /**
   * Returns version and properties for base install
   *
   * @return VersionInfo
   */
  public VersionInfo getVersionInfo() {
    return new VersionInfo(productInfo);
  }

  /**
   * Returns version and properties for the core install package
   *
   * @return VersionInfo
   */
  public VersionInfo getCoreVersionInfo() {
    return getVersionInfo(CORE_PACKAGE);
  }

  /**
   * Returns version and properties for install package
   *
   * @param installPkg - String
   * @return VersionInfo
   */
  public VersionInfo getVersionInfo(String installPkg) {
    VersionInfo version = null;
    File pkgMarker = new File(installHome + "/install/tar/." + installPkg);

    // package marker exists; load it
    if (pkgMarker.exists()) {
      try {
        Properties props = new Properties();
        FileInputStream in = new FileInputStream(pkgMarker);
        props.load(in);
        in.close();

        // add in edition
        props.setProperty(PersistencePath.EDITION_NAME, editionName);
        props.setProperty(PersistencePath.EDITION_SHORT_NAME, editionshortname);

        version = new VersionInfo(props);
      } catch (IOException ioe) {
        Trace.logError(ioe.getMessage(), ioe);
      }
    }

    return version;
  }

  /**
   * Get a list of all versions installed to the same directory.
   *
   * @throws
   */
  public List<StreamsInstall> getAllVersions() {
    return null;
  }

  /**
   * Returns a list of all version directories associated with install
   *
   * @return List<File>
   */
  public List<File> getAllVersionDirectories() {
    List<File> versionDirs = new ArrayList<File>();
    Pattern pattern = Pattern.compile(VERSION_PATTERN);
    File installDir = new File(installHome);
    File installBase = installDir.getParentFile();

    // all version directories are siblings
    for (File subDir : installBase.listFiles()) {
      if (subDir.isDirectory()) {
        Matcher matcher = pattern.matcher(subDir.getName());
        if (matcher.matches()) {
          versionDirs.add(subDir);
        }
      }
    }

    return versionDirs;
  }

  /**
   * Returns true if package name is install, false if not.
   *
   * @return true or false
   */
  public boolean isPackageInstalled(String pkgName) {
    File pkgFile = new File(installHome + "/install/tar/" + pkgName);

    // package file exists
    if (pkgFile.exists()) {
      return true;
    } else {
      return false;
    }
  }

  /**
   * Returns all of the product properties
   *
   * @return Properties
   */
  public Properties getProductInfo() {
    return productInfo;
  }

  /**
   * Returns the product name contained in the .product file for this install.
   *
   * @return The product name.
   */
  public String getProductName() {
    return name;
  }

  /**
   * Returns the product version contained in the .product file for this install.
   *
   * @return The product version.
   */
  public String getProductVersion() {
    return version;
  }

  /**
   * Returns the product build information contained in the .product file for this install.
   *
   * @return The build version.
   */
  public String getBuildVersion() {
    return build;
  }

  /**
   * Returns the edition information contained in the .product file for this install.
   *
   * @return The edition of the product.
   */
  public String getEditionName() {
    return editionName;
  }

  /**
   * Returns the edition short name contained in the .product file for this install.
   *
   * @return The edition of the product. (Product, Internal, Developer, etc..)
   */
  public String getEditionShortName() {
    return editionshortname;
  }

  /**
   * Returns the hotfix information contained in the .product file for this install.
   *
   * @return The hot fix level of the installed product. (Most times this is empty string since its
   *     rare to have a hotfix installed)
   */
  public String getHotFixVersion() {
    return hotFix;
  }

  /**
   * Returns the architecture supported information contained in the .product file for this install.
   *
   * @return The architecture supported.
   */
  public String getArchitecture() {
    return arch;
  }

  /**
   * Returns the OS version string contained in the .product file for this install.
   *
   * @return The OS version string.
   */
  public String getOS() {
    return osStr;
  }

  /**
   * Returns os major version (i.e. 5, 6 )
   *
   * @return String os major version
   */
  public String getOSVersion() {
    return osVersion;
  }

  /**
   * Returns os minor version (i.e. 5.6 returns 6 )
   *
   * @return String
   */
  public String getOSPatch() {
    return osPatch;
  }

  /**
   * Retrieve the product file for this install.
   *
   * @return The product file.
   */
  public File getProductFile() {
    return prodFile;
  }

  /**
   * Retrieve the license file for this install.
   *
   * @return List<File>
   */
  public List<File> getLicenseFiles() {
    List<File> files = new ArrayList<File>();

    // .product
    files.add(prodFile);

    // Example of swidtag file
    // For VRM: ./swidtag/ibm.com_IBM_Streams-4.1.1.swidtag
    // For fixpack, interim or hotfix: none
    File swidtagdir = new File(installHome, ISO_READINESS_DIR);

    File[] swidtagfiles =
        swidtagdir.listFiles(
            new FilenameFilter() {
              public boolean accept(File dir, String name) {
                return name.toLowerCase().endsWith(".swidtag");
              }
            });

    if (swidtagfiles != null) {
      for (File f : swidtagfiles) {
        files.add(f);
      }
    }

    return files;
  }

  /**
   * Returns the path to the Streams install base directory. This path does NOT include the specific
   * version sub-directory
   *
   * @return String
   */
  public String getBaseInstallPath() {
    return installHome.substring(0, installHome.lastIndexOf('/'));
  }

  /**
   * Returns the path to the Streams install for this install. This path includes the version
   * sub-directory.
   *
   * @return The install path.
   */
  public String getInstallPath() {
    return installHome;
  }

  /**
   * Check for the existence of public and private key files for SSH.
   *
   * @return True if both files exist, false if one or both files does not exist.
   */
  public boolean keyFilesExist() {
    String userHome = System.getProperty("user.home");
    String user = System.getProperty("user.name");
    File pubKeyFile = new File(userHome + "/.streams/key/" + user + ".pem");
    File privKeyFile = new File(userHome + "/.streams/key/" + user + "_priv.pem");

    return privKeyFile.exists() && pubKeyFile.exists();
  }

  /**
   * ************************************************************************************ NOTE: if
   * you change this method you also have to change the equivalent method in
   * Other\INST\InstallProject\src\com\ibm\streams\inst\common\SecurityEncryptionFiles.java
   * **************************************************************************************
   */
  public boolean isJCEInstalled() {
    boolean localInstalled = false;
    boolean usInstalled = false;
    try {
      byte[] lBytes =
          Files.readAllBytes(Paths.get(installHome + "/java/jre/lib/security/local_policy.jar"));
      byte[] lHash = MessageDigest.getInstance("MD5").digest(lBytes);
      String lChecksum = DatatypeConverter.printHexBinary(lHash);
      if (lChecksum.equals(localPolicyChecksum)) {
        localInstalled = true;
      }
    } catch (IOException ioe) {
      Trace.logError("Failed to calculate checksum for local policy file", ioe);
    } catch (Throwable t) {
      Trace.logError("Unexpected error calculating checksum for local policy file", t);
    }

    try {
      byte[] usBytes =
          Files.readAllBytes(
              Paths.get(installHome + "/java/jre/lib/security/US_export_policy.jar"));
      byte[] usHash = MessageDigest.getInstance("MD5").digest(usBytes);
      String usChecksum = DatatypeConverter.printHexBinary(usHash);
      if (usChecksum.equals(usExportPolicyChecksum)) {
        usInstalled = true;
      }
    } catch (IOException ioe) {
      Trace.logError("Failed to calculate checksum for US export policy file", ioe);
    } catch (Throwable t) {
      Trace.logError("Unexpected error calculating checksum for US export policy file", t);
    }

    return (localInstalled | usInstalled);
  }

  /**
   * ************************************************************************************ NOTE: if
   * you change this method you also have to change the equivalent method in
   * Other\INST\InstallProject\src\com\ibm\streams\inst\common\SecurityEncryptionFiles.java
   * **************************************************************************************
   */
  public void createJCEPackage() throws NotWritableException, StreamsException, IOException {
    File tarDir = new File(installHome, "/install/tar");
    if (tarDir.exists() && tarDir.isDirectory() && tarDir.canWrite()) {
      Path srcPath = Paths.get(installHome + "/install/tar/.streamscore");
      Path tgtPath = Paths.get(installHome + "/install/tar/.streamsjce");
      Files.copy(srcPath, tgtPath, StandardCopyOption.REPLACE_EXISTING);

      // set owner and group of file based on parent directory
      Path tarPath = Paths.get(installHome + "/install/tar");
      UserPrincipal owner = Files.getOwner(tarPath);
      GroupPrincipal group =
          Files.readAttributes(tarPath, PosixFileAttributes.class, LinkOption.NOFOLLOW_LINKS)
              .group();
      Files.setOwner(tgtPath, owner);
      Files.getFileAttributeView(tgtPath, PosixFileAttributeView.class, LinkOption.NOFOLLOW_LINKS)
          .setGroup(group);

      try {
        String[] tgzCmd = {
          "/bin/tar",
          "-zcf",
          installHome + "/install/tar/streamsjce.tgz",
          "-C",
          installHome,
          "./java/jre/lib/security/local_policy.jar",
          "./java/jre/lib/security/US_export_policy.jar",
          "./install/tar/.streamsjce"
        };
        ProcessBuilder tgzBuilder = new ProcessBuilder(tgzCmd);
        tgzBuilder.redirectErrorStream(true);
        Process tgzProc = tgzBuilder.start();
        int exitValue = -1;
        boolean completed = false;
        while (!completed) {
          try {
            exitValue = tgzProc.waitFor();
            completed = true;
          } catch (InterruptedException ie) {
            // Trace.logError("Failed to create streamsjce.tgz in install/tar", ie);
            // throw new StreamsException(ie);
          }
        }
        BufferedReader rdr = new BufferedReader(new InputStreamReader(tgzProc.getInputStream()));
        String line;
        if (exitValue == 0) {
          line = rdr.readLine();
          InternalUtilities.cleanupProcess(tgzProc);

          // set owner and group for file
          Path tgzPath = Paths.get(installHome + "/install/tar/streamsjce.tgz");
          Files.setOwner(tgzPath, owner);
          Files.getFileAttributeView(
                  tgzPath, PosixFileAttributeView.class, LinkOption.NOFOLLOW_LINKS)
              .setGroup(group);
          Set<PosixFilePermission> tgzPermissions = new HashSet<PosixFilePermission>();
          tgzPermissions.add(PosixFilePermission.GROUP_READ);
          tgzPermissions.add(PosixFilePermission.GROUP_EXECUTE);
          tgzPermissions.add(PosixFilePermission.OTHERS_READ);
          tgzPermissions.add(PosixFilePermission.OTHERS_EXECUTE);
          tgzPermissions.add(PosixFilePermission.OWNER_READ);
          tgzPermissions.add(PosixFilePermission.OWNER_WRITE);
          tgzPermissions.add(PosixFilePermission.OWNER_EXECUTE);
          Files.setPosixFilePermissions(tgzPath, tgzPermissions);

          // symlink to tgz
          Path linkPath = Paths.get(installHome + "/StreamsDomainHost/streamsjce.tgz");
          if (linkPath.toFile().getParentFile().exists()) {
            if (!linkPath.toFile().exists()) {
              Files.createSymbolicLink(linkPath, tgzPath);
              // setowner does not set the owner of symlink, so we'll use the -h command for chown
              String[] chownCmd = {
                "/usr/bin/chown",
                "-h",
                owner.getName(),
                installHome + "/StreamsDomainHost/streamsjce.tgz"
              };
              ProcessBuilder pb = new ProcessBuilder(chownCmd);
              pb.start();
              Files.getFileAttributeView(
                      linkPath, PosixFileAttributeView.class, LinkOption.NOFOLLOW_LINKS)
                  .setGroup(group);
            }
          } else {
            if (Trace.isEnabled(Level.DEBUG)) {
              Trace.logDebug("Skipping symlink because StreamsDomainHost does not exist.");
            }
          }
        } else {
          if (Trace.isEnabled(Level.DEBUG)) {
            Trace.logDebug("Failed create streamsjce.tgz in install/tar exit=" + exitValue);
          }
          while ((line = rdr.readLine()) != null) {
            if (Trace.isEnabled(Level.DEBUG)) {
              Trace.logDebug(line);
            }
          }
          InternalUtilities.cleanupProcess(tgzProc);
          throw new IOException(line);
        }
      } catch (IOException ioe) {
        try {
          Files.deleteIfExists(tgtPath);
        } catch (IOException ioe2) {
          Trace.logError("Failed to clean up JCE marker file.", ioe2);
        }
        throw ioe;
      } catch (Throwable t) {
        try {
          Files.deleteIfExists(tgtPath);
        } catch (IOException ioe) {
          Trace.logError("Failed to clean up JCE marker file.", ioe);
        }
        throw t;
      }
    } else {
      NotWritableException nwe = new NotWritableException(Key.ERR_PATH_NOT_WRITE, tarDir.getPath());
      throw nwe;
    }
  }

  /**
   * ************************************************************************************ NOTE: if
   * you change this method you also have to change the equivalent method in
   * Other\INST\InstallProject\src\com\ibm\streams\inst\common\SecurityEncryptionFiles.java
   * **************************************************************************************
   */
  public void createJCEPackageFromInstall(StreamsInstall streamsInstall)
      throws IOException, NotWritableException, StreamsException {
    if (streamsInstall.isJCEInstalled()) {
      if (Trace.isEnabled(Level.DEBUG)) {
        Trace.logDebug(
            "##### creating JCE install package: version="
                + getProductVersion()
                + " from existing version="
                + streamsInstall.getProductVersion());
      }

      // save off old
      Path srcLocalPolicy =
          Paths.get(streamsInstall.getInstallPath() + "/java/jre/lib/security/local_policy.jar");
      Path tgtLocalPolicy = Paths.get(installHome + "/java/jre/lib/security/local_policy.jar");
      Path oldLocalPolicy = Paths.get(installHome + "/java/jre/lib/security/local_policy.jar.old");
      Files.copy(tgtLocalPolicy, oldLocalPolicy, StandardCopyOption.REPLACE_EXISTING);

      Path srcExportPolicy =
          Paths.get(
              streamsInstall.getInstallPath() + "/java/jre/lib/security/US_export_policy.jar");
      Path tgtExportPolicy = Paths.get(installHome + "/java/jre/lib/security/US_export_policy.jar");
      Path oldExportPolicy =
          Paths.get(installHome + "/java/jre/lib/security/US_export_policy.jar.old");
      Files.copy(tgtExportPolicy, oldExportPolicy, StandardCopyOption.REPLACE_EXISTING);

      try {
        // copy policy files to current install
        Files.copy(srcLocalPolicy, tgtLocalPolicy, StandardCopyOption.REPLACE_EXISTING);
        Files.copy(srcExportPolicy, tgtExportPolicy, StandardCopyOption.REPLACE_EXISTING);

        createJCEPackage();
      } catch (IOException ioe) {
        Trace.logError("Failed to copy policy files to current install at " + installHome, ioe);
        // restore old policy files
        try {
          Files.copy(oldLocalPolicy, tgtLocalPolicy, StandardCopyOption.REPLACE_EXISTING);
          Files.copy(oldExportPolicy, tgtExportPolicy, StandardCopyOption.REPLACE_EXISTING);
        } catch (IOException ioe2) {
          Trace.logError("Failed to restore old policy files", ioe2);
        }
      } finally {
        // clean up temp files
        Files.deleteIfExists(oldLocalPolicy);
        Files.deleteIfExists(oldExportPolicy);
      }
    }
  }
}
