======================================================================== 
ex9a.c: Semaphore

    Created on: Jan 15, 2018
          Author: eliyho tsuri, Dan Narkisi
            Login: eliyhots , danna
                 Id: 201610672, 301680435
 
DESCRIOPTION:
      The parent process assigns sheard memory and gives birth to two child processes, 
      each of the children in a loop from a number scroller to a prime number lottery.
      During the process (the child) grill the prime number he stores it in sheard memory ,
      kills his brother, and sends a signal to his parent, and finishes.
      The parent displays the number, and ends as well.
 
Compile: gcc -o ex9a ex9a.c -lpthread -lrt
Run: ex9a 

Input: not input 
Output : The rational number

======================================================================== 
הערה לבודק : ניסינו לקמפל עם הפקודה שרשומה בספר של יורם וזה לא הלך, מצאנו פקודה אחרת שאיתה הצלחנו לקמפל
הפקודה נלקחה מהאתר :http://www.csc.villanova.edu/~mdamian/threads/posixsem.html 


