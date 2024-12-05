# hobo-vr 2
electric boogaloo

currently connecting clients can only pick from 2 device types, and the available device types are
pretty bare bones and rough, but for testing thats enough

if you want you can add more device types, its relatively easy to do

just write the device impl class that inherits from `IHvrTrackedDevice`, add that type to the
supported device types array, add its init to the provider class switch with the rest of em, and
you're pretty much done, the rest doesn't need any change

## building
```bash
mkdir build
cd build
cmake ..
cmake --build .
```
