# WtExtensions Library CMAKE file

CMAKE_MINIMUM_REQUIRED(VERSION 3.14 FATAL_ERROR)

PROJECT(WtExtensions DESCRIPTION "Wt Extensions")

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-fPIC")

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

set(SOURCES
  source/extensions.cpp
  source/extendedProgressBar.cpp
  source/loggerSink.cpp
  source/moneyValidator.cpp
  source/progressGroup.cpp
  source/progressText.cpp
  source/requirementsWidget.cpp
  )
set(HEADERS
  WtExtensions
  include/extendedComboBox.h
  include/extendedDoubleSpinBox.h
  include/extendedLineEdit.h
  include/extendedProgressBar.h
  include/extendedPushButton.h
  include/extendedTimeEdit.h
  include/extensions.h
  include/fileListWidget.h
  include/loggerSink.h
  include/moneyValidator.h
  include/progressGroup.h
  include/progressText.h
  include/requirementsWidget.h
  include/stream2Control.h
  )

set(INCLUDES
  ${CMAKE_SOURCE_DIR}/GCL
  ${CMAKE_SOURCE_DIR}/SCL
  ${CMAKE_SOURCE_DIR}/fmt/include
  ${CMAKE_SOURCE_DIR}/wt/src
  ${CMAKE_SOURCE_DIR}/build/wt
  )

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})


target_include_directories(${PROJECT_NAME}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${Wt_INCLUDE_DIRECTORY} ${INCLUDES}
  )
