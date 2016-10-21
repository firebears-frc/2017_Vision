# 2017_Vision
Vision code for 2846.

# Building
The first dependency is lib-aldaron, which gives a graphical interface to the program and a way to capture webcam or usb camera input.  You can pull it with

```
git clone https://github.com/OxyDeadbeef/lib-aldaron
```

To build lib aldaron, cd into lib-aldaron/ and type:

```
make
```

On windows, this fails partway through - so you have to switch from Git BASH to Command Prompt after it fails, and re-run the makefile.

The second dependency is CCV (You can switch back to GIT BASH on Windows), you can download by cd-ing into 2017_Vision/ and running

```
make gitaclib
```

Next we need to link with lib-aldaron.  To do this type

```
make
```

To build the test target type:

```
make test --silent
```

To build release target type:

```
make release --silent
```
