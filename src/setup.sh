docker build . -t weird_vm
docker run --name chall -p 1337:1337 -dit --privileged weird_vm
