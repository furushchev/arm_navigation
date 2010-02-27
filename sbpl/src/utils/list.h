/*
 * Copyright (c) 2008, Maxim Likhachev
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of Pennsylvania nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __LIST_H_
#define __LIST_H_

//the maximum size of the heap
#define LISTSIZE 5000000

struct listelement
{
  AbstractSearchState *liststate;
  struct listelement *prev;
  struct listelement *next;
};
typedef struct listelement listelement; 

class CList
{

//data
public:

	listelement* firstelement;
	listelement* lastelement;
	int currentsize;

//constructors
public:
	CList() 
	  {
	    firstelement = NULL;
		lastelement = NULL;
	    currentsize = 0;
	  };
	~CList()
	{};

//functions
public:
  bool empty()
    {return (currentsize == 0);};
  bool full()
    {return (currentsize >= LISTSIZE);};
  bool in(AbstractSearchState *AbstractSearchState1, int listindex)
    { 
      return (AbstractSearchState1->listelem[listindex] != NULL);
    };
  void makeemptylist(int listindex)
    {
      while(firstelement != NULL)
		remove(firstelement->liststate, listindex);
    };

void insert(AbstractSearchState *AbstractSearchState1, int listindex)
{
	if(currentsize >= LISTSIZE)
	{
		printf("ERROR: list is full\n");
		exit(1);
	}
	if(AbstractSearchState1->listelem[listindex] != NULL)
	{
		printf("ERROR: insert: element is already in the list\n");
		exit(1);
	}
	listelement *insertelem = (listelement*)malloc(sizeof(listelement));
	insertelem->liststate = AbstractSearchState1;
	insertelem->prev = NULL;
	insertelem->next = firstelement;
	if(firstelement != NULL)
		firstelement->prev = insertelem;
	firstelement = insertelem;
	if(lastelement == NULL) //if this is the first element to be inserted into the list
		lastelement = insertelem;
	AbstractSearchState1->listelem[listindex] = insertelem;
	currentsize++;
};

void insertinfront(AbstractSearchState *AbstractSearchState1, int listindex)
    {
      insert(AbstractSearchState1, listindex);
    };

void remove(AbstractSearchState *AbstractSearchState1, int listindex)
{
	if(currentsize == 0 || AbstractSearchState1->listelem[listindex] == NULL)
	{
		printf("ERROR: delete: list does not contain the element\n");
		exit(1);
	}
	if(AbstractSearchState1->listelem[listindex]->prev != NULL && 
		AbstractSearchState1->listelem[listindex]->next != NULL)
	{
		//in the middle of the list
		AbstractSearchState1->listelem[listindex]->prev->next = 
		AbstractSearchState1->listelem[listindex]->next;
		AbstractSearchState1->listelem[listindex]->next->prev = 
		AbstractSearchState1->listelem[listindex]->prev;
	}
	else if(AbstractSearchState1->listelem[listindex]->prev != NULL)
	{
		//at the end of the list
		AbstractSearchState1->listelem[listindex]->prev->next = NULL;
		lastelement = AbstractSearchState1->listelem[listindex]->prev;
	}
	else if(AbstractSearchState1->listelem[listindex]->next != NULL)
	{
		//at the beginning of the list
		AbstractSearchState1->listelem[listindex]->next->prev = NULL;
		firstelement = AbstractSearchState1->listelem[listindex]->next;
	}
	else
	{
		//the only element in the list
		firstelement = NULL;
		lastelement = NULL;
	}
	//delete
	free(AbstractSearchState1->listelem[listindex]);
	AbstractSearchState1->listelem[listindex] = NULL;
	currentsize--;
};

  
AbstractSearchState *getfirst()
{
	if(firstelement == NULL)
		return NULL;
	else
		return firstelement->liststate;
};

AbstractSearchState *getlast()
{
	if(lastelement == NULL)
		return NULL;
	else
		return lastelement->liststate;
};


AbstractSearchState *getnext(AbstractSearchState* AbstractSearchState1, int listindex)
{
	if(AbstractSearchState1->listelem[listindex]->next == NULL)
		return NULL;
	else
		return AbstractSearchState1->listelem[listindex]->next->liststate;
};

private:



//operators
public:


};


class CBucket
{

//data
public:
	//buckets are from first_priority to numofbuckets-3. Bucket numofbucket-2 contains elements with higher finite priorities (unordered). Last bucket contains infinite
	//priority elements
	vector<AbstractSearchState *>* bucketV;
	vector<int> assortedpriorityV; //contains the priorities of elements in the numofbuckets-2 bucket (mixed priorities) 
	int firstpriority;
	int numofbuckets;
	int currentminelement_bucketind;
	int currentminelement_priority;
	int currentminelement_bucketVind; //index within the bucket

	int maxassortedpriorityVsize;

//constructors
public:
	CBucket(int first_priority, int max_bucketed_priority) 
	 {
		bucketV = NULL;
		firstpriority = 0;
		numofbuckets = 0;
		currentminelement_bucketind = INFINITECOST;
		currentminelement_priority = INFINITECOST;
		currentminelement_bucketVind = INFINITECOST;
		maxassortedpriorityVsize = 0;

		reset(first_priority, max_bucketed_priority);
	 };
	~CBucket()
	{
		if(bucketV != NULL)
		{
			makeemptybucketV();

			delete [] bucketV;
			bucketV = NULL;
			firstpriority = 0;
			numofbuckets = 0;

		}
	};

//functions
public:
	bool empty()
	{
		return (currentminelement_bucketind == INFINITECOST);
	};

	bool reset(int first_priority, int max_bucketed_priority)
	{
		//delete the old ones
		if(numofbuckets != 0)
		{
			makeemptybucketV();

			delete [] bucketV;
			bucketV = NULL;
			firstpriority = 0;
			numofbuckets = 0;
		}

		//compute the number of buckets
		numofbuckets = max_bucketed_priority - first_priority + 1; //this is how many priorities are one per buckets
		numofbuckets += 2; //one bucket for (max_bucketed_priority; infinity) and one bucket for infinity

		//allocate memory
		bucketV = new vector<AbstractSearchState *>[numofbuckets];

		//currently all empty
		currentminelement_bucketind = INFINITECOST;
		currentminelement_priority = INFINITECOST;
		currentminelement_bucketVind = INFINITECOST;

		//reset statistics
		maxassortedpriorityVsize = 0;

		return true;
	};

	void makeemptybucketV()
	{
		for(int i = 0; i < numofbuckets; i++)
		{
		  for(int eind = 0; eind < (int)bucketV[i].size(); eind++)
			{
				bucketV[i].at(eind)->heapindex = -1;
			}
		}
		assortedpriorityV.clear(); //clear the assorted priorities array

		//currently all empty
		currentminelement_bucketind = INFINITECOST;
		currentminelement_priority = INFINITECOST;
		currentminelement_bucketVind = INFINITECOST;

	};

	AbstractSearchState *getminelement()
	{
		if(currentminelement_bucketind == INFINITECOST)
			return NULL;
#if DEBUG
		else if(currentminelement_bucketind >= numofbuckets)
		{	
			printf("ERROR: currentminelement_bucketind is invalid\n");
			exit(1);
		}
		else if((int)bucketV[currentminelement_bucketind].size() <= currentminelement_bucketVind)
		{
			printf("ERROR: failed to get minelement\n");
			exit(1);
		}
#endif
		else
		{
			return bucketV[currentminelement_bucketind].at(currentminelement_bucketVind);
		}

	};

	AbstractSearchState *popminelement()
	{
		if(currentminelement_bucketind == INFINITECOST)
			return NULL;
#if DEBUG
		else if(currentminelement_bucketind >= numofbuckets)
		{	
			printf("ERROR: currentminelement_bucketind is invalid\n");
			exit(1);
		}
		else if((int)bucketV[currentminelement_bucketind].size() <= currentminelement_bucketVind)
		{
			printf("ERROR: failed to get minelement\n");
			exit(1);
		}
#endif
		else
		{
			AbstractSearchState* AbstractSearchState1 = bucketV[currentminelement_bucketind].at(currentminelement_bucketVind);
			removestategivenbucketindex(AbstractSearchState1, currentminelement_bucketind);
			return AbstractSearchState1;
		}

	};


	int getminpriority()
	{
		return currentminelement_priority;
	};
	void remove(AbstractSearchState *AbstractSearchState1, int priorityattimeofinsertion)
	{
		//compute bucketindex
		int bucketindex = priorityattimeofinsertion - firstpriority;
		if(priorityattimeofinsertion == INFINITECOST)
			bucketindex = numofbuckets-1;
		else if (bucketindex > numofbuckets-2)
			bucketindex = numofbuckets-2;

		removestategivenbucketindex(AbstractSearchState1, bucketindex);
	};

	void insert(AbstractSearchState *AbstractSearchState1, int priority)
	{
		//compute bucket index
		int bucketindex = priority - firstpriority;
		if(priority == INFINITECOST)
		{
			bucketindex = numofbuckets - 1;
		}
		else if(bucketindex >= numofbuckets - 2)
		{
			bucketindex = numofbuckets - 2; //overflow bucket
		}
		
		//insert the element itself
		bucketV[bucketindex].push_back(AbstractSearchState1);
		AbstractSearchState1->heapindex = (int)bucketV[bucketindex].size()-1;

		if(bucketindex == numofbuckets - 2)
		{
			assortedpriorityV.push_back(priority);

			if(maxassortedpriorityVsize < (int)assortedpriorityV.size())
			  maxassortedpriorityVsize = (int)assortedpriorityV.size();
		}

		//re-compute minelement if necessary
		if(priority < currentminelement_priority || currentminelement_priority == INFINITECOST)
		{
			recomputeminelementupfrombucket(bucketindex);
		}
	};
	

private:

	void removestategivenbucketindex(AbstractSearchState *AbstractSearchState1, int bucketindex)
	{

		int removeelemind = AbstractSearchState1->heapindex;
		int lastelemind = (int)bucketV[bucketindex].size()-1;

		//put into its place the last element in the bucket
		if(bucketindex == numofbuckets-2)
			assortedpriorityV.at(removeelemind) = assortedpriorityV.at(lastelemind);
		bucketV[bucketindex].at(lastelemind)->heapindex = removeelemind;
		bucketV[bucketindex].at(removeelemind) = bucketV[bucketindex].at(lastelemind);

		//clear the element itself
		AbstractSearchState1->heapindex = -1;

		//remove the last element
		bucketV[bucketindex].pop_back();

		//re-compute minelement if necessary
		if(bucketindex == currentminelement_bucketind && ((int)bucketV[bucketindex].size() == 0 || currentminelement_bucketind == numofbuckets - 2))
		{
			recomputeminelementupfrombucket(bucketindex);
		}
	};


	void recomputeminelementupfrombucket(int startbucketindex)
	{
		//first find non-empty bucket
		int bind = -1;
		for (bind = startbucketindex; bind < numofbuckets; bind++)
		{
		  if((int)bucketV[bind].size() != 0)
				break;
		}

		if(bind < numofbuckets - 2)
		{
			//normal bucket
			currentminelement_bucketind = bind;
			currentminelement_priority = firstpriority + bind;
			currentminelement_bucketVind = 0;
		}
		else if(bind == (numofbuckets -2))
		{
			//assorted priorities
			currentminelement_bucketind = bind;
			currentminelement_priority = INFINITECOST;
			currentminelement_bucketVind = INFINITECOST;
			for(int eind = 0; eind < (int)bucketV[bind].size(); eind++)
			{
				if(currentminelement_priority > assortedpriorityV.at(eind))
				{
					currentminelement_priority = assortedpriorityV.at(eind);
					currentminelement_bucketVind = eind;
				}
			}
			if(currentminelement_priority == INFINITECOST)
			{
				printf("ERROR: in recomputemin in buckets\n");
				exit(1);
			}
		}
		else if(bind == (numofbuckets - 1))
		{
			//bucket with infinite priorities
			currentminelement_bucketind = bind;
			currentminelement_priority = INFINITECOST;
			currentminelement_bucketVind = 0;
		}
		else
		{
			currentminelement_bucketind = INFINITECOST;
			currentminelement_priority = INFINITECOST;
			currentminelement_bucketVind = INFINITECOST;
		}

	};

};


class CSlidingBucket
{

//data
public:
	//fixed size buckets. Each bucket is of size bucketsize, and there are numofbuckets buckets.
	//all unoccupied buckets are supposed to be set to NULL
	AbstractSearchState*** bucketV;
	int* lastelementindexV; //index of the last element for each bucket
	int numofbuckets;
	int bucketsize;
	int currentminbucket_index; //index of the current minbucket
	int currentminelement_index; //index of the current minelement in the bucket
	int currentmaxpriority; //the max priority in the list
	int currentminpriority; //the priority of the currentminbucket


//constructors
public:
	CSlidingBucket(int num_of_buckets, int bucket_size) 
	 {
		numofbuckets = num_of_buckets;
		bucketsize = bucket_size;
		
		//allocate memory
		bucketV = new AbstractSearchState** [numofbuckets];
		lastelementindexV = new int[numofbuckets];
		for(int i = 0; i < numofbuckets; i++)
		{
			lastelementindexV[i] = -1;
			bucketV[i] = NULL; 
		}
	
		currentminbucket_index = 0;
		currentminelement_index = -1;
		currentmaxpriority = currentminpriority = 0;
	 };
	~CSlidingBucket()
	{
		for(int i = 0; i < numofbuckets; i++)
		{
			if(bucketV[i] != NULL)
			{
				delete [] bucketV[i];
				bucketV[i] = NULL;
			}
		}
		delete [] bucketV;
		bucketV = NULL;
		delete [] lastelementindexV;
	};

//functions
public:

  inline bool empty()
    {return (currentminelement_index == -1 && currentmaxpriority == currentminpriority);};

  inline int getminkey()
  	{return currentminpriority;};

  void reset()
  {
		currentminbucket_index = 0;
		currentminelement_index = -1;
		currentmaxpriority = currentminpriority = 0;
		for(int i = 0; i < numofbuckets; i++)
		{
			lastelementindexV[i] = -1;
			if(bucketV[i] == NULL)
				continue;
			for(int eind = 0; eind < bucketsize; eind++)
				bucketV[i][eind] = NULL;
		}

  };

	AbstractSearchState *popminelement()
	{
		if(currentminelement_index == -1 && currentmaxpriority == currentminpriority)
			return NULL;
		else
		{
			AbstractSearchState* currentelement = NULL;
			if(currentminelement_index == -1 || bucketV[currentminbucket_index] == NULL || bucketV[currentminbucket_index][currentminelement_index] == NULL)
				currentelement = recomputeandreturnmin();
			else
				currentelement = bucketV[currentminbucket_index][currentminelement_index];

			//delete the element
			bucketV[currentminbucket_index][currentminelement_index] = NULL;

			//recomputemin
			recomputeandreturnmin();

			return currentelement;
		}
	};

	AbstractSearchState *getminelement()
	{
		if(currentminelement_index == -1 && currentmaxpriority == currentminpriority)
			return NULL;
		else
		{
			AbstractSearchState* currentelement = NULL;
			if(currentminelement_index == -1 || bucketV[currentminbucket_index] == NULL || bucketV[currentminbucket_index][currentminelement_index] == NULL)
				currentelement = recomputeandreturnmin();
			else
				currentelement = bucketV[currentminbucket_index][currentminelement_index];

			return currentelement;
		}
	};

	void insert(AbstractSearchState *AbstractSearchState1, int priority)
	{
		//compute the index of the bucket where to put it in
		int bucket_increment = (priority - currentminpriority);
		int bucket_index = (currentminbucket_index + bucket_increment)%numofbuckets;

		if(bucket_increment >= numofbuckets || bucket_increment < 0)
		{
			printf("ERROR: invalid priority=%d (currentminpriority=%d) used with sliding buckets\n", priority, currentminpriority);
			exit(1);
		}

		//insert the element
		lastelementindexV[bucket_index]++;

		if(lastelementindexV[bucket_index] == bucketsize)
		{
			printf("ERROR: bucket %d is full (size=%d)\n", bucket_index, bucketsize);
			exit(1);
		}

		if(bucketV[bucket_index] == NULL)
			createbucket(bucket_index);

		bucketV[bucket_index][lastelementindexV[bucket_index]] = AbstractSearchState1;

		//make sure maximum is correct
		if(priority > currentmaxpriority)
			currentmaxpriority = priority;

		//special case for the only entry
		if(currentminbucket_index == bucket_index && currentminelement_index == -1)
		{
			currentminelement_index = 0;
		}
	};

private:

	AbstractSearchState *recomputeandreturnmin()
	{
		if(currentminelement_index == -1 && currentmaxpriority == currentminpriority)
			return NULL;
		while(currentminelement_index == -1 || bucketV[currentminbucket_index] == NULL || bucketV[currentminbucket_index][currentminelement_index] == NULL)
		{
			//try incrementing element index
			if (currentminelement_index < lastelementindexV[currentminbucket_index])
				currentminelement_index++;
			else
			{
				//there are no more elements left in this bucket
				lastelementindexV[currentminbucket_index] = -1;

				//if it was the last bucket, then that is it
				if(currentminpriority == currentmaxpriority)
				{
					currentminbucket_index = 0;
					currentminelement_index = -1;
					currentmaxpriority = currentminpriority;
					return NULL;
				}

				//try incrementing bucket index
				currentminbucket_index = (currentminbucket_index + 1)%numofbuckets;
				currentminelement_index = 0;
				currentminpriority++;
			}
		}
		return bucketV[currentminbucket_index][currentminelement_index];
	};

	void createbucket(int bucketindex)
	{
		if(bucketV[bucketindex] != NULL)
		{
			printf("ERROR: trying to create a non-null bucket\n");
			exit(1);
		}

		bucketV[bucketindex] = new AbstractSearchState* [bucketsize];
		for(int eind = 0; eind < bucketsize; eind++)
			bucketV[bucketindex][eind] = NULL;
	};

};


#endif
