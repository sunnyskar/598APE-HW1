# 598APE-HW1

## Build

- Pulling the code:

```bash
git clone https://github.com/dhanker2/598APE-HW1.git
```

- Start off with docker installed.

- Build the docker image with the following command in the root directory:

```bash
sudo docker build -t test docker/
```

- Run the docker container with the following command:

```bash
sudo ./dockerrun.sh
```

- Go to the host directory:

```bash
cd host/ 
```


## Run the program
The branch that the program is on has only the parallelization optimizations. If you want to test the bvh optimization, you will need to checkout the branch as defined below.

- Build the project with the following command:

```bash
make clean && make - j
```


There are 3 commands to test:
```bash
# piano room
./main.exe -i inputs/pianoroom.ray --ppm -o output/pianoroom.ppm -H 500 -W 500

# globe
./main.exe -i inputs/globe.ray --ppm  -a inputs/globe.animate --movie -F 24 

# elephant
./main.exe -i inputs/elephant.ray --ppm  -a inputs/elephant.animate --movie -F 24 -W 100 -H 100 -o output/sphere.mp4 
```

## Testing Optimizations:

To test the optimizations, you will need to run the program under the branches(checkout the branch, then run the program):

- No Optimizations:
```bash

git checkout f0fa0139ee5ca4e541bcc122ba4394d5906fb7ab
```

- Memory Optimizations:
```bash
git checkout 10ff1782222fd4e67db6a560deae63ee6c5a5d12
```

- Flag Optimizations:
```bash
git checkout 648505a824f162b215332a184c0b0a3b9c56dd9e
```

- Parallelization Optimizations:
```bash
git checkout main
```

- BVH Optimizations:
```bash
git checkout bvh
```







