QT += widgets

HEADERS       = tetrixboard.h \
                ai.h \
                game.h \
                tetrixpiece.h \
                tetrixwindow.h
SOURCES       = main.cpp \
                ai.cpp \
                file.cpp \
                game.cpp \
                tetrixboard.cpp \
                tetrixpiece.cpp \
                tetrixwindow.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/tetrix
INSTALLS += target
