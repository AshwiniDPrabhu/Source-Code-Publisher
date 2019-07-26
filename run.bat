cd Debug
start ServerPrototype.exe 8080
cd ..
cd GUI\bin\Debug
start CodePublisher.exe 8082 8080
timeout 5
start CodePublisher.exe 9002 8080
cd ../../..