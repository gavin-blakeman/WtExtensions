#-----------------------------------------------------------------------------------------------------------------------------------
#
# PROJECT:            Wt Extensions
# FILE:								WtExtensions.pro
# SUBSYSTEM:          Project File
# LANGUAGE:						C++
# TARGET OS:          LINUX
# LIBRARY DEPENDANCE:	None.
# NAMESPACE:          N/A
# AUTHOR:							Gavin Blakeman.
# LICENSE:            GPLv2
#
#                     Copyright 2019, 2022 Gavin Blakeman.
#                     This file is part of the WtExtensions Library
#
#                     WtExtensions is free software: you can redistribute it and/or modify it under the terms of the GNU General
#                     Public License as published by the Free Software Foundation, either version 2 of the License, or (at your
#                     option) any later version.
#
#                     WtExtensions is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
#                     implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
#                     for more details.
#
#                     You should have received a copy of the GNU General Public License along with WtExtensions.  If not, see
#                     <http://www.gnu.org/licenses/>.
#
# OVERVIEW:						Project file for compiling the project
#
# HISTORY:            2019-10-11/GGB - Development of WtExtensions
#
#-----------------------------------------------------------------------------------------------------------------------------------

TARGET = WtExtensions
TEMPLATE = lib
CONFIG += staticlib

QT       -= core gui

QMAKE_CXXFLAGS += -std=c++20
DEFINES += BOOST_THREAD_USE_LIB

unix:CONFIG(release, debug|release) {
  DESTDIR = ""
  OBJECTS_DIR = "objects"
}
else:unix:CONFIG(debug, debug|release) {
  DESTDIR = ""
  OBJECTS_DIR = "objects"
}

INCLUDEPATH += \
#  "../boost 1.62" \
#  "../cfitsio" \
#  "../dlib-19.4" \
  "../GCL" \
#  "../libWCS/wcstools-3.8.7/libwcs" \
#  "../MCL" \
#  "../NOVAS" \
#  "../PCL" \
#  "../SBIG" \
#  "../SCL" \
#  "../SOFA/src" \
#  "../" \
#  "../GeographicLib/GeographicLib-1.48/include/GeographicLib"

SOURCES += \
    source/extensions.cpp \
    source/loggerSink.cpp \
    source/moneyValidator.cpp

HEADERS += \
  WtExtensions \
  include/extendedComboBox.h \
  include/extendedDoubleSpinBox.h \
  include/extendedLineEdit.h \
  include/extendedPushButton.h \
  include/extendedTimeEdit.h \
  include/extensions.h \
  include/loggerSink.h \
  include/moneyValidator.h


OTHER_FILES += \
    license.txt \
    README \
    changelog.txt
