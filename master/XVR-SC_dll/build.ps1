$cmd = $args[0]

if ($cmd -eq "c") {
     cd source
     make
     cd ..
}

if ($cmd -eq "clean") {
     cd source
     make clean
     cd ..
}

if ($cmd -eq "b") {
     cd source
     make clean
     make
     make b
     cd ..
}

if ($cmd -eq "bw") {
     cd source
     make clean
     make
     make bw
     cd ..
}
