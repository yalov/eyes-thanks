#----------------------------------------------------------------------------------#
#      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  #
#      This file is part of Eyes' Thanks.                                          #
#      GNU General Public License 3                                                #
#----------------------------------------------------------------------------------#

defineTest(removeDirRecursive) {
    DIR_TO_DEL = $$shell_path($$1)
    RETURN = $$escape_expand(\n\t)
    QMAKE_POST_LINK += $$RETURN $$QMAKE_DEL_TREE $$quote($$DIR_TO_DEL)
    export(QMAKE_POST_LINK)
}

defineTest(removeFiles) {
        FILES_TO_DEL = $$shell_path($$1) # full path or ex.
        RETURN = $$escape_expand(\n\t)
        for(FILE,FILES_TO_DEL){
            QMAKE_POST_LINK += $$RETURN $$QMAKE_DEL_FILE $$quote($$FILE)
        }

    export(QMAKE_POST_LINK)
}

defineTest(createDir) {
    DIR = $$shell_path($$1)
    RETURN = $$escape_expand(\n\t)
    QMAKE_POST_LINK += $$RETURN $$sprintf($$QMAKE_MKDIR_CMD, $$DIR)
    export(QMAKE_POST_LINK)
}

defineTest(copyFilesToDESTDIR) {
        FILES_TO_COPY = $$shell_path($$1) # full path or ex.
        DDIR = $$shell_path($$DESTDIR)
        RETURN = $$escape_expand(\n\t)
        for(FILE,FILES_TO_COPY){
            QMAKE_POST_LINK += $$RETURN $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR)
        }

    export(QMAKE_POST_LINK)
}

defineTest(windeployqtInDESTDIR) {
    ARGS = $$1
    RETURN = $$escape_expand(\n\t)
    QMAKE_POST_LINK += $$RETURN windeployqt $$ARGS $$quote($$shell_path($$DESTDIR))
    export(QMAKE_POST_LINK)
}

defineTest(StartProjectMESSAGE) {
    win32 {
        BUILD_TIME = $$system("echo %time:~0,8%")
    } else {
        BUILD_TIME = $$system("time")
    }
    message("")
    message("$$BUILD_TIME start .pro-file")
}
