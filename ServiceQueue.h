
#ifndef _SERVICE_QUEUE_H
#define _SERVICE_QUEUE_H

#include <iostream>
#include <vector>
#include <utility>

class ServiceQueue {

private:
	struct Node {
		int buzzer;
		Node *next;
		Node *back;

		Node(double _buzzer=-1, Node* _next=nullptr, Node* _back=nullptr){
		buzzer = _buzzer ; 
		next = _next;
		back = _back;
		}
	};

	Node* head;		// first buzzer in the queue
	Node* last; 	// last buzzer in the queue
	int size; 		// size of the queue
	std::vector<int> buzzer_bucket; // stack of reuseable buzzers
	std::vector<Node*> access_buzzer; // list of node pointers for each buzzer that has been used 

public:

/**
 * Constructor
 * Description: intializes an empty service queue.
 * 
 * RUNTIME : O(1)
 *
 * */
ServiceQueue() {
	head = nullptr;
	last = nullptr;
	size = 0;
}

/**
 * Destructor
 * Description:  deallocates all memory assciated
 *   with service queue 
 *
 * RUNTIME :  O(N_b) where N_b is the number of buzzer 
 *	IDs that have been used during the lifetime of the
*	service queue; in general, at any particular instant
*	the actual queue length may be less than N_b.
*
*	[See discussion of "re-using buzzers" below]
*
*  */
~ServiceQueue() {
	Node *p, *pnxt;
	p = head;
	while(p != nullptr) {
		pnxt = p->next;
		delete p;
		p = pnxt;
	}
	size = 0;
	head = nullptr;
	last = nullptr;
}

/**
 * Function: snapshot()
 * param:  buzzers is an integer vector passed by ref
 * Description:  populates buzzers vector with a "snapshot"
 *               of the queue as a sequence of buzzer IDs 
 *
 *
 * RUNTIME:  O(N)  (where N is the current queue
 *		length).
*/
void snapshot(std::vector<int> &buzzers) {
	// clear the vector first
	buzzers.clear();   

	Node* tmp = head;
	while(tmp != nullptr) {
		buzzers.push_back(tmp->buzzer);
		tmp = tmp->next;
	}
}

/**
 * Function: length()
 * Description:  returns the current number of
 *    entries in the queue.
 *
 * RUNTIME :  O(1)
 */
int  length() {
	return size;
}

/**
 * Function: give_buzzer()
 * Return:   buzzer-ID (integer) assigned to the new customer.
 * Description:  This is the "enqueue" operation.  For us
 *    a "buzzer" is represented by an integer (starting
 *    from zero).  The function selects an available buzzer 
 *    and places a new entry at the end of the service queue 
 *    with the selected buzer-ID. 
 *    This buzzer ID is returned.
 *    The assigned buzzer-ID is a non-negative integer 
 *    with the following properties:
 *
 *       (1) the buzzer (really it's ID) is not currently 
 *         taken -- i.e., not in the queue.  (It
 *         may have been in the queue at some previous
 *         time -- i.e., buzzer can be re-used).
 *	  This makes sense:  you can't give the same
*	  buzzer to two people!
*
*       (2) Reusable Buzzers:  A re-usable buzzer is 
*	  a buzzer that _was_ in the queue at some previous
*	  time, but currently is not.
*
	*         REQUIREMENT:  If there is one or more reusable
	*         buzzer, you MUST return one of them; furthermore,
	*         it must be the buzzer that became reusable most 
	*         MOST RECENTLY.
*
*       (3) if there are no previously-used / reusable buzzers, 
	*         the SMALLEST possible buzzer-ID is used (retrieved from 
	*         inventory).  
*	    Properties in this situation (where N is the current
*	      queue length):
*
*		- The largest buzzer-ID used so far is N-1
*
*		- All buzzer-IDs in {0..N-1} are in the queue
*			(in some order).
*
*		- The next buzzer-ID (from the basement) is N.
*
*
* RUNTIME :  O(1)  ON AVERAGE or "AMORTIZED"  
*          In other words, if there have been M calls to 
*		give_buzzer, the total time taken for those 
*		M calls is O(M).
*
*/
int give_buzzer() {
	int b;
	Node* add_buzzer = new Node();

	// check for reusable buzzers
	if(buzzer_bucket.size() != 0) {
		b = buzzer_bucket.back();
		buzzer_bucket.pop_back();
		add_buzzer->buzzer = b;
		access_buzzer[b] = add_buzzer;
	}
	else{
		b = length();
		add_buzzer->buzzer = b;
		access_buzzer.push_back(add_buzzer);
	}

	// check if the queue is empty
	if( head == nullptr){
		head = add_buzzer;
		last = head;
		add_buzzer->back = head;
		size++;
	}
	else{
		last->next = add_buzzer;
		add_buzzer->back = last;
		last = add_buzzer;
		size++;
	}
	return b;
}

/**
 * function: seat()
 * description:  if the queue is non-empty, it removes the first 
 *	 entry from (front of queue) and returns the 
*	 buzzer ID.
*	 Note that the returned buzzer can now be re-used.
*
*	 If the queue is empty (nobody to seat), -1 is returned to
*	 indicate this fact.
	*
	* Returns:  buzzer ID of dequeued customer, or -1 if queue is empty.
*
* RUNTIME :  O(1)
*/
int seat() {
	int b;
	Node* erase = head;

	if(length() == 0) 
		return -1;

	if(length() == 1){
		b = head->buzzer;
		buzzer_bucket.push_back(b);
		access_buzzer[b] = nullptr;
		head = nullptr;
		last = nullptr;
		delete erase;
		size--;
	}
	else {
		b = head->buzzer;
		head = head->next;
		buzzer_bucket.push_back(b);
		access_buzzer[b] = nullptr;
		delete erase;
		size--;
	}
	return b;
}

/**
 * function: kick_out()
 *
 * description:  Some times buzzer holders cause trouble and
 *		a bouncer needs to take back their buzzer and
*		tell them to get lost.
*
*		Specifially:
*
*		If the buzzer given by the 2nd parameter is 
*		in the queue, the buzzer is removed (and the
*		buzzer can now be re-used) and 1 (one) is
*		returned (indicating success).
*
* Return:  If the buzzer isn't actually currently in the
*		queue, the queue is unchanged and false is returned
*		(indicating failure).  Otherwise, true is returned.
*
* RUNTIME :  O(1)
*/
bool kick_out(int buzzer) {
	// check if the buzzer was ever given out
	if (access_buzzer.size() < buzzer)
		return false;

	Node* erase = access_buzzer[buzzer];

	// check if the buzzer is in the queue
	if (erase == nullptr){
		return false;
	}
	// if it is the only buzzer in the queue
	if(length() == 1){
		head = nullptr;
		last = nullptr;
		buzzer_bucket.push_back(buzzer);
		access_buzzer[buzzer] = nullptr;
		delete erase;
		size--;
	}
	// if it is the first buzzer in the queue
	else if (head == erase){
		int kicked = seat(); // remove normally in a queue
	}
    
	// if the buzzer is at the end of the queue
	else if (last == erase){
		last = erase->back;
		last->next = nullptr;
		buzzer_bucket.push_back(buzzer);
		access_buzzer[buzzer] = nullptr;
		delete erase;
		size--;
	}
	// if buzzer is somewhere in the middle of queue
	else{
		erase->back->next = erase->next;
		erase->next->back = erase->back;
		buzzer_bucket.push_back(buzzer);
		access_buzzer[buzzer] = nullptr;
		delete erase;
		size--;
	}
	return true;
}

/**
 * function:  take_bribe()
 * description:  some people just don't think the rules of everyday
 *		life apply to them!  They always want to be at
*		the front of the line and don't mind bribing
*		a bouncer to get there.
*
*	        In terms of the function:
*
*		  - if the given buzzer is in the queue, it is 
*		    moved from its current position to the front
*		    of the queue.  1 is returned indicating success
*		    of the operation.
*		  - if the buzzer is not in the queue, the queue 
*		    is unchanged and 0 is returned (operation failed).
*
* Return:  If the buzzer isn't actually currently in the
*		queue, the queue is unchanged and false is returned
*		(indicating failure).  Otherwise, true is returned.
*
* RUNTIME :  O(1)
*/
bool take_bribe(int buzzer) {
	// check if the buzzer was ever used
	if (access_buzzer.size() < buzzer)
		return false;

	Node* bribed = access_buzzer[buzzer];

	// check if the buzzer is in the queue
	if (bribed == nullptr){
		return false;
	}
	// check if buzzer is already at the front of queue
	else if(head == bribed){
		return true;
	}
	// check if buzzer is at the end of queue
	else if(last == bribed){
		last = bribed->back;
		last->next = nullptr;
		bribed->back = nullptr;
		bribed->next = head;
		head = bribed;
		bribed->next->back = bribed;
		return true;
	}
	// if its in the middle of the queue
	else{
		bribed->back->next = bribed->next;
		bribed->next->back = bribed->back;
		bribed->next = head;
		head = bribed;
		bribed->next->back = bribed;
	}
	return true;
}
};   // end ServiceQueue class

#endif

