# WtExtensions Library CMAKE file

CMAKE_MINIMUM_REQUIRED(VERSION 3.14 FATAL_ERROR)

PROJECT(WtExtensions DESCRIPTION "Wt Extensions")

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-fPIC")

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

find_package(Wt REQUIRED)

set(SOURCES
  source/extensions.cpp
  source/loggerSink.cpp
  source/moneyValidator.cpp
  source/progressText.cpp
  )
set(HEADERS
  WtExtensions
  include/extendedComboBox.h
  include/extendedDoubleSpinBox.h
  include/extendedLineEdit.h
  include/extendedPushButton.h
  include/extendedTimeEdit.h
  include/extensions.h
  include/loggerSink.h
  include/moneyValidator.h
  include/progressText.h
  include/stream2Control.h
  )

set(INCLUDES
  ${CMAKE_SOURCE_DIR}/GCL
  ${CMAKE_SOURCE_DIR}/SCL
  )

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})


target_include_directories(${PROJECT_NAME}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${Wt_INCLUDE_DIRECTORY} ${INCLUDES}
  )
