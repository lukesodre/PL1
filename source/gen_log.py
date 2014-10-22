__author__ = 'daniel'
import subprocess
import time
import sys

def runProcess(exe):    
    p = subprocess.Popen(exe, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    while(True):
      retcode = p.poll() #returns None while subprocess is running
      line = p.stdout.readline()
      yield line
      if(retcode is not None):
        break


f = open('log.txt', 'w')
f.write('Log:\n')
f.close()

orig_stdout = sys.stdout
f = open('log.txt', 'w')
sys.stdout = f

##### START #####

# Select 1
millis_ant = int(round(time.time() * 1000))
print("Select1 Start")

for x in range(0, 99):
    print("./select newHeap "+str(x)+" AA ZZ 2000")
    for line in runProcess(("./select newHeap "+str(x)+" AA ZZ 2000").split()):
        print(line)

print("Select1 End")
time_select = str(int(round(time.time() * 1000))-millis_ant)

# Select 2
millis_ant = int(round(time.time() * 1000));
print("Select2 Start")

for x in range(0, 99):
    print("./select2 newHeap "+str(x)+" AA ZZ 2000")
    for line in runProcess(("./select2 newHeap "+str(x)+" AA ZZ 2000").split()):
        print(line)

print("Select2 End")
time_select2 = str(int(round(time.time() * 1000))-millis_ant)

# Select 3 - Different attribute
millis_ant = int(round(time.time() * 1000))
print("Select3 Start - Different attribute")

for x in range(0, 99):
    y = x + 1
    print("./select3 newHeap "+str(x)+" "+str(y)+" AA ZZ 2000")
    for line in runProcess(("./select3 newHeap "+str(x)+" "+str(y)+" AA ZZ 2000").split()):
        print(line)

print("Select3 End - Different attribute")
time_select3_same = str(int(round(time.time() * 1000))-millis_ant)


# Select 3 - Same attribute
millis_ant = int(round(time.time() * 1000))
print("Select3 Start - Same attribute")

for x in range(0, 99):
    print("./select3 newHeap "+str(x)+" AA ZZ 2000")
    for line in runProcess(("./select3 newHeap "+str(x)+" "+str(x)+" AA ZZ 2000").split()):
        print(line)

print("Select3 End - Same attribute")
time_select3_diff = str(int(round(time.time() * 1000))-millis_ant)

print("Select1 time in milliseconds: " + time_select)
print("Select2 time in milliseconds: " + time_select2)
print("Select3 same attribute time in milliseconds: " + time_select3_same)
print("Select3 different attribute time in milliseconds: " + time_select3_diff)

#### END ####
sys.stdout = orig_stdout
f.close()