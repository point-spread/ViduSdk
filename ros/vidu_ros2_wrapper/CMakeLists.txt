cmake_minimum_required(VERSION 3.5)
project(vidu_ros2_wrapper)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

find_package(rclcpp REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(image_transport REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)

find_package(OpenCV REQUIRED)
message(STATUS ${OpenCV_INCLUDE_DIRS} " are found")
message(STATUS $ENV{DYVCAM_GENTL64_PATH}/../cmake/ " is found")
find_package(Vidu_SDK_Api REQUIRED HINTS $ENV{DYVCAM_GENTL64_PATH}/../cmake)
find_package(vidu_ros2_interfaces REQUIRED)

include_directories(include/  ${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}_node src/vidu_ros2_wrapper.cpp)
target_link_libraries(${PROJECT_NAME}_node 
  # vidu_ros2_interfaces // there is no .so for the vidu_ros2_interfaces, it only have headers
  Vidu_SDK_Api::GenTLwrapper 
  ${OpenCV_LIBS} 
)
ament_target_dependencies(${PROJECT_NAME}_node
 rclcpp cv_bridge image_transport std_msgs sensor_msgs 
 std_srvs  tf2 tf2_ros vidu_ros2_interfaces  
)

install(TARGETS
  ${PROJECT_NAME}_node
  DESTINATION lib/${PROJECT_NAME})
  
ament_package()
