# voice-control-to-move-the-most-basic-ros2

## do some replacement and install

PLZ use this,

[a program ro implement speech to text](https://github.com/mgonzs13/whisper_ros)

, and then renew the ros package file `src/whisper_ros/whisper_demos/whisper_demos/whisper_demo_node.py` && `src/whisper_ros/whisper_demos/CMakeLists.txt` && `src/whisper_ros/whisper_demos/package.xml` with the file here.

cd to your workspace and the copy this to your terminal

```bash
colcon build --symlink-install --cmake-args -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=release -DGGML_CUDA=ON
```

## launch

```bash
source install/setup.bash
ros2 launch xyz xyz.launch.py

```

You can use this to observe

```bash
ros2 topic echo /cmd_vel
```

## And then you can use your voice to control the robot

