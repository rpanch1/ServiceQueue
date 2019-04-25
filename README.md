# ServiceQueue
Data Structures - Project 2

For this project, I had to implement a Service Queue class.
When you go to a restaurant that doesn't take reservations, you have to wait in line
for a table. Most restaturants will giv eyou a buzzer which will signal you when your table is ready.
So, this class basically manages a queue of buzzers with these given member functions.

- give_buzzer: 
  this is the enqueue operation (a new customer is getting in line).
  the following happens: An unused buzzer ID is determined and an entry for that buzzer is added to
  the end of the queue and the selected buzzer ID is returned. runtime: constant.
  
- seat: 
  This is the dequeue operation. If the queue is not empty, the first entry is removed from the front; the
  removed buzzer ID can now be re-used. runtime: constant.
  
- take_bribe:
  A person holding a particular buzzer may offer the system a bribe to move to the front of the queue.       This function takes a buzzer ID and does the following: If the buzzer is indeed somewhere in the queue,   it is plucked out and moved to the front of the queue and true is returned. Otherwise, return false.
  runtime: constant.
  
- kick_out:
  This operation takes a buzzer ID and does the following: 
  If the buzzer is indeed somewhere in the queue, it is removed and true is returned.  The buzzer now       becomes reusable (again, see Buzzer Policy). Otherwise return false. runtime: constant.
  
- snapshot:
  This function takes an integer vector as a reference parameter and populates it with the current queue     contents (i.e., sequence of buzzer IDs) in sequence. runtime: linear.
  
- length: 
   reportes the curent length of the queue. runtime: constant.
   
