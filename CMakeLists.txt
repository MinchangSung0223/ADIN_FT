cmake_minimum_required(VERSION 3.10)

# 프로젝트 이름 설정
project(ADIN_FT)
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})
# 표준 설정
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
SUBDIRS(bullet3)
# Include 디렉토리 설정
include_directories(${PROJECT_SOURCE_DIR}/include)

# lib 디렉토리 설정
link_directories(${PROJECT_SOURCE_DIR}/lib)
file(GLOB_RECURSE FTread src/FTread.cpp src/FTread.h)

# src 디렉토리 내부의 모든 .cpp 파일들을 찾아서 변수에 저장
file(GLOB SOURCES "src/main.cpp" ${FTread})

# 실행 파일 이름 및 소스 지정
add_executable(main ${SOURCES})

# 라이브러리 링크
# 이 부분은 실제 라이브러리 이름에 맞게 변경해주세요
target_link_libraries(main canlib)
target_link_libraries(main  pybullet_cpp)



