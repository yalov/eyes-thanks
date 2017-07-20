#----------------------------------------------------------------------------------#
#      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  #
#      This file is part of Eyes' Thanks.                                          #
#      GNU General Public License 3                                                #
#----------------------------------------------------------------------------------#

win32 {
    BUILD_TIME = $$system("echo %time:~0,8%")
} else {
    BUILD_TIME = $$system("time")
}

# Recursive remove directory
defineTest(removeDirRecursive) {
    DIR_TO_DEL = $$shell_path($$1)
    RETURN = $$escape_expand(\n\t)
    QMAKE_POST_LINK += $$RETURN $$QMAKE_DEL_TREE $$quote($$DIR_TO_DEL)
    export(QMAKE_POST_LINK)
}

# Remove some files
defineTest(removeFiles) {
    FILES_TO_DEL = $$shell_path($$1) # full path (split spaces) or mask *
    RETURN = $$escape_expand(\n\t)
    for(FILE, FILES_TO_DEL){
        QMAKE_POST_LINK += $$RETURN $$QMAKE_DEL_FILE $$quote($$FILE)
    }
    export(QMAKE_POST_LINK)
}

# Remove some files from directory (directory_path and file_names)
defineTest(removeFilesInDir) {
    PATH = $$shell_path($${1}) # full path to directory
    FILENAMES = $${2}          # filenames inside directory for remove
    RETURN = $$escape_expand(\n\t)
    for(FILENAME, FILENAMES){
        QMAKE_POST_LINK += $$RETURN $$QMAKE_DEL_FILE $$quote($${PATH}$${FILENAME})
    }
    export(QMAKE_POST_LINK)
}

# Create empty directory
defineTest(createDir) {
    DIR = $$shell_path($$1)
    RETURN = $$escape_expand(\n\t)
    QMAKE_POST_LINK += $$RETURN $$sprintf($$QMAKE_MKDIR_CMD, $$DIR)
    export(QMAKE_POST_LINK)
}

# create directory if not exist, then copy some files to that directory
defineTest(copyFilesToDir) {
    FILES = $$shell_path($$1)  # full filepath (split spaces) or mask *
    DIR = $$shell_path($$2)    # directory path
    RETURN = $$escape_expand(\n\t)
    QMAKE_POST_LINK += $$RETURN $$sprintf($$QMAKE_MKDIR_CMD, $$DIR)
    for(FILE,FILES){
        QMAKE_POST_LINK += $$RETURN $$QMAKE_COPY $$quote($$FILE) $$quote($$DIR)
    }
    export(QMAKE_POST_LINK)
}

# windeployqt in $$DESTDIR with some ARGS
defineTest(windeployqtInDESTDIR) {
    ARGS = $$1
    RETURN = $$escape_expand(\n\t)
    QMAKE_POST_LINK += $$RETURN windeployqt $$ARGS $$quote($$shell_path($$DESTDIR))
    export(QMAKE_POST_LINK)
}
