# @PWD Specifies the full path
# leading to the directory
# containing the _current file_ being parsed
#INCLUDEPATH += $$PWD


LIBS += -L$$OUT_PWD/../qutfstring/ -lqutfstring


INCLUDEPATH += $$PWD/../qutfstring
DEPENDPATH += $$PWD/../qutfstring

win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../qutfstring/libqutfstring.a
else:win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../qutfstring/qutfstring.lib

