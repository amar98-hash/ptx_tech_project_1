import serial
import numpy as np
import string

port = serial.Serial('/dev/cu.usbmodem14301', 115200)
abc=open('abc.txt', 'r+b')  #append and binary(a+b) mode
img_data=np.zeros((64, 80))

img_data_string=[]

for i in range(0, 63):
   x= port.readline().rstrip()     #1 byte
   len(x)
   y=x.decode("unicode_escape")
   z=y.split(" ")
   #img_data[i][0:79]=int(x)
  
   img_data[i][0:80]=np.asarray(z)

print(img_data)   

   
   



abc.close()
port.close()