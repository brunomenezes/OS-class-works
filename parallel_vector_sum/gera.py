import random
f1=open('a_d.in', 'w')
f2=open('b_d.in', 'w')
for i in range(1000000): f1.write(str(random.uniform(-10,+10))+'\n')
for i in range(1000000): f2.write(str(random.uniform(-10,+10))+'\n')
f1.close()
f2.close()
