
projects="map"

for i in `ls *.pro`
do 
    echo Building $i...
    qmake $i
    make 2>&1 | grep error 
done

exit 0
