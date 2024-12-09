# cmake_minimum_required(VERSION 3.5)
# project(my_nav_goal_publisher)

# find_package(ament_cmake REQUIRED)
# find_package(rclcpp REQUIRED)
# find_package(rclcpp_action REQUIRED)
# find_package(geometry_msgs REQUIRED)
# find_package(nav2_msgs REQUIRED)

# add_executable(my_nav_goal_publisher src/my_nav_goal_publisher.cpp)

# ament_target_dependencies(my_nav_goal_publisher
#   rclcpp
#   rclcpp_action
#   geometry_msgs
#   nav2_msgs
# )

# install(TARGETS
#   my_nav_goal_publisher
#   DESTINATION lib/${PROJECT_NAME}
# )

# ament_package()

cmake_minimum_required(VERSION 3.5)
project(my_nav_goal_publisher)

# 设置 C++ 版本
set(CMAKE_CXX_STANDARD 17)

# 查找 ROS 2 和所需依赖包
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav2_msgs REQUIRED)

# 手动设置 BehaviorTreeCPP_v3 包路径
set(BehaviorTreeCPP_v3_DIR "/opt/ros/humble/share/behaviortree_cpp_v3/cmake")
set(CMAKE_PREFIX_PATH "/opt/ros/humble/share/behaviortree_cpp_v3/cmake" ${CMAKE_PREFIX_PATH})


# 查找 BehaviorTreeCPP_v3 包
find_package(behaviortree_cpp_v3 REQUIRED)

# 包含头文件目录
include_directories(include)

# 定义可执行文件
add_executable(my_nav_goal_publisher src/test_tree.cpp)

# 连接库
ament_target_dependencies(my_nav_goal_publisher
  rclcpp
  rclcpp_action
  geometry_msgs
  nav2_msgs
  behaviortree_cpp_v3
)

# 安装可执行文件
install(TARGETS my_nav_goal_publisher
  DESTINATION lib/${PROJECT_NAME}
)

# 导出包
ament_package()
