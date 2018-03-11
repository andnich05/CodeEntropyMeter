HEADERS += \
    include/BitDisplay.hpp \
    include/Entropy.hpp \
    include/EntropyDisplay.hpp \
    include/InfoWindow.hpp \
    include/MainWindow.hpp \
    include/MeterDisplay.hpp \
    include/OptionPanel.hpp \
    include/PeakMeter.hpp \
    include/PortAudioControl.hpp \
    include/PortAudioIO.hpp \
    include/RingBuffer.hpp \
    include/RMSMeter.hpp

SOURCES += \
    src/BitDisplay.cpp \
    src/Entropy.cpp \
    src/EntropyDisplay.cpp \
    src/InfoWindow.cpp \
    src/Main.cpp \
    src/MainWindow.cpp \
    src/MeterDisplay.cpp \
    src/OptionPanel.cpp \
    src/PeakMeter.cpp \
    src/PortAudioControl.cpp \
    src/PortAudioIO.cpp \
    src/RingBuffer.cpp \
    src/RMSMeter.cpp




TEMPLATE = app

TARGET = code-entropy-meter

QT += widgets \

win32: LIBS += -L$$PWD/lib/portaudio -lportaudio_x86 -lportaudio_x64
unix: LIBS += -L$$PWD/lib/portaudio -lportaudio -lasound

INCLUDEPATH += $$PWD/include
INCLUDEPATH += $$PWD/include/portaudio
DEPENDPATH += $$PWD/include/portaudio
