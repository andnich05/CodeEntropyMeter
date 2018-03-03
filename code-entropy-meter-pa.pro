HEADERS += \
    include\PortAudioIO.hpp \
    include\RingBuffer.hpp \
    include\PortAudioControl.hpp \
    include\BitDisplay.hpp \
    include\RMSMeter.hpp \
    include\PeakMeter.hpp \
    include\MainWindow.hpp \
    include\OptionPanel.hpp \
    include\Entropy.hpp \
    include\EntropyDisplay.hpp \
    include\MeterDisplay.hpp \
    include\InfoWindow.hpp

SOURCES += \
    src\Main.cpp \
    src\PortAudioIO.cpp \
    src\RingBuffer.cpp \
    src\PortAudioControl.cpp \
    src\BitDisplay.cpp \
    src\RMSMeter.cpp \
    src\PeakMeter.cpp \
    src\MainWindow.cpp \
    src\OptionPanel.cpp \
    src\Entropy.cpp \
    src\EntropyDisplay.cpp \
    src\MeterDisplay.cpp \
    src\InfoWindow.cpp



TEMPLATE = app

TARGET = code-entropy-meter

QT += widgets \

win32: LIBS += -L$$PWD/lib/portaudio -lportaudio_x86 -lportaudio_x64

INCLUDEPATH += $$PWD/include
INCLUDEPATH += $$PWD/include/portaudio
DEPENDPATH += $$PWD/include/portaudio
