#include "main.h"

class imp_res : public Restaurant
{
	customer* X = NULL;				//customer X at the table
	customer* frontQ = NULL;		//customer in the waiting queue, front
	customer* curQ = NULL;			//customer in the waiting queue, rear
	customer* removeList = NULL;	//a list to hold the order of cus to be removed, at table
	int cusCount = 0;				//number of cus at the table
	int waitCount = 0;				//number of cus in the waiting queue

	customer* generalList = NULL;	//a list to hold the order of all customers arriving at the restaurant
	int totalCount = 0;				//number of all customers arriving at the restaurant

	public:	
		imp_res() {};

		//check if cus name already existed at table or in queue
		bool checkName(customer* head, string cusName) {
			if(head == NULL) return false;
			customer* temp = head;
			bool check = false;
			do {
				if(temp->name == cusName) {
					check = true;
					break;
				}
				temp = temp->next;
			}
			while(temp != NULL);
			// delete temp;
			return check;
		}

		//add cus to generalList, no matter what
		void addOne(customer* cus) {
			if(totalCount == 0) {
				generalList = new customer(cus->name, cus->energy, NULL, NULL);
				totalCount++;
			}
			else {
				customer* temp = new customer(cus->name, cus->energy, NULL, NULL);
				generalList->next = temp;
				temp->prev = generalList;
				generalList = temp;
				totalCount++;
			}
		} 

		void RED(string name, int energy) {
			// cout << name << " " << energy << endl;
			customer *cus = new customer (name, energy, nullptr, nullptr);
			// cout << MAXSIZE << endl;

			if(energy == 0) {
				// delete cus;
				return;
			}

			// table is full, add to waiting queue
			if(cusCount == MAXSIZE) {
				if(waitCount == 0) {
					if(!checkName(generalList, name)) addOne(cus);
					this->frontQ = cus;
					this->curQ = cus;
					this->frontQ->next = NULL;
					this->frontQ->prev = NULL;
					waitCount++;
					// delete cus;
					return;
				}
				else if(waitCount <= MAXSIZE) {
					if(checkName(frontQ, name) || checkName(removeList, name)) {
						// delete cus;
						return;
					}
					if(!checkName(generalList, name)) addOne(cus);
					customer* temp = this->frontQ;
					while (temp->next != NULL) {
						temp = temp->next;
					}
					temp->next = cus;
					cus->prev = temp;
					this->curQ = cus;
					waitCount++;
					// delete temp;
					// delete cus;
					return;					
				}
				else {
					// delete cus;
					return;
				}
			}
			
			// table is not yet full
			// table has no customer
			if(cusCount == 0) {
				if(!checkName(generalList, name)) addOne(cus);
				this->X = cus;
				X->next = X;
				X->prev = X;
				removeList = new customer(cus->name, cus->energy, NULL, NULL);
				cusCount++;
				// delete cus;
				return;
			}
			// if table already has customers, check if name exists in table and queue
			else {
				if(checkName(frontQ, name) || checkName(removeList, name)) {
					// delete cus;
					return;
				}
				if(!checkName(generalList, name)) addOne(cus);
				if(cusCount >= (MAXSIZE / 2)) {
					// burh burh, hehe copycat
					int RES = -1;
					int pRES;
					customer* temp = this->X;
					customer* hold;
					do{
						int pRES = cus->energy - temp->energy;
						if(abs(RES) < abs(pRES)){
							RES = pRES;
							hold = temp;
						}
						temp = temp->next;
					} 
					while(temp != this->X);

					if(RES >= 0){
						cus->next = hold->next;
						hold->next->prev = cus;
						hold->next = cus;
						cus->prev = hold;
					}
					else{
						cus->prev = hold->prev;
						hold->prev->next = cus;
						hold->prev = cus;
						cus->next = hold;
					}
					// delete temp;
					// delete hold;
				}
				else {
					if(cus->energy >= this->X->energy) {
						cus->prev = this->X;
						this->X->next->prev = cus;
						cus->next = this->X->next;
						this->X->next = cus;
					}
					else {
						cus->next = this->X;
						cus->prev = this->X->prev;
						this->X->prev->next = cus;
						this->X->prev = cus;
					}
				}
				
				cusCount++;
				this->X = cus;
				customer* temp = removeList;
				while (temp->next != NULL) {
					temp = temp->next;
				}
				temp->next = new customer(cus->name, cus->energy, temp, NULL);
				// delete temp;
				// delete cus;
				return;
			}
		}

		//add customer to table, remove them from queue
		void addFromQueue(customer* cus) {
			if(cus == nullptr) return;
			customer* temp = new customer(cus->name, cus->energy, nullptr, nullptr);
			remove(frontQ, temp->name, waitCount);
			removeGen(generalList, temp->name, totalCount);
			RED(temp->name, temp->energy);
		}

		void BLUE(int num)
		{
			// cout << "blue "<< num << endl;
			if(num >= MAXSIZE) num = MAXSIZE;

			while(num != 0) {
				customer* temp = this->X;
				while(temp->name != removeList->name) {
					temp = temp->next;
				}
				removeGen(generalList, temp->name, totalCount);
				
				if(temp->energy > 0) this->X = temp->next;
				else this->X = temp->prev;
				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;
				temp->next = NULL;
				temp->prev = NULL;
				// delete temp;

				customer* cur = removeList;
				removeList = removeList->next;
				cur->next = NULL;
				cur->prev = NULL;
				// delete cur;
				cusCount--;
				num--;
			}

			customer* add = frontQ;
			while((cusCount < MAXSIZE) && (waitCount > 0)) {
				addFromQueue(add);
				add = frontQ;
			}
		}

		//swap 2 customers in purple sort
		void swap(customer *&z, customer *&y){
			string a = z->name;
			int b = z->energy;

			z->name = y->name;
			z->energy = y->energy;

			y->name = a;
			y->energy = b;
		}

		//get customer by index, for purple sort
		customer* getCustomerByIndex(customer* head, int index) {
			customer* temp = head;
			for(int i = 0; i < index; i++)
				temp = temp->next;
			return temp;
		}

		//get index by customer, for purple sort
		int getIndexByCustomer(customer* head, customer* cus) {
			customer* temp = head;
			int index = 0;
			while(temp->name != cus->name) {
				temp = temp->next;
				index++;
			}
			return index;
		}

		//sort va tra ve so lan swap
		int purpleSort(customer* head, customer* front, int useSize) {
			int N = 0;
			for(int gap = useSize / 2; gap > 0; gap /= 2) {
				for(int seg = 0; seg < gap; seg++) {
					//insertion sort
					for(int i = seg; i < useSize; i += gap) {
						if(i - gap >= 0) {
							int k = i;
							bool flag = true;
							while(flag) {
								if(k - gap < 0) break;
								flag = false;
								customer* a = getCustomerByIndex(head, k);
								customer* b = getCustomerByIndex(head, k - gap);
								if(abs(a->energy) > abs(b->energy)) {
									swap(a, b);
									N++;
									flag = true;
									k -= gap;
								}
								else if((abs(a->energy) == abs(b->energy))){
									if(((getIndexByCustomer(front, a) < getIndexByCustomer(front, b)))) {
										swap(a, b);
										N++;
										flag = true;
										k -= gap;
									}
								}
							} 
						}
					}
				}
			}
			return N;
		}

		void PURPLE()
		{
			// cout << "purple"<< endl;
			if(frontQ == NULL) return;

			int useSize, counting = 0;
			int maxEnergy = 0;
			customer* temp = this->frontQ;
			customer* hold;
			while(temp != NULL) {
				counting++;
				if(abs(temp->energy) >= abs(maxEnergy)) {
					maxEnergy = abs(temp->energy);
					hold = temp;
					useSize = counting;
				}
				temp = temp->next;
			}

			//DEEPCOPY CHO HÀNG CHỜ THÀNH PURPLE LIST
			temp = this->frontQ;
			customer* purpleList = nullptr;
			customer* tmp = nullptr;
			int check = 0;
			while (check < useSize) {
				if (purpleList == nullptr) {
					purpleList = new customer(temp->name, temp->energy, nullptr, nullptr);
					tmp = purpleList;
				} else {
					tmp->next = new customer(temp->name, temp->energy, tmp, nullptr);
					tmp = tmp->next;
				}
				temp = temp->next;
				check++;
			}
			int N = purpleSort(purpleList, this->frontQ, useSize);
			BLUE(N % MAXSIZE);
		}

		void REVERSAL()
		{
			// cout << "reversal" << endl;
			customer* reverseList1 = NULL;
            customer* reverseList2 = NULL;
            customer* currentCus = this->X;
            customer* nextCus = this->X->prev;
			string nameX = this->X->name;

            if(this->X->energy < 0) {
                do {
					if(nextCus->energy > 0) {
						customer* cus = new customer(nextCus->name, nextCus->energy, NULL, NULL);
						if(reverseList1 == NULL) reverseList1 = cus;
						else {
							reverseList1->next = cus;
							cus->prev = reverseList1;
							reverseList1 = cus;
						}
					}
					nextCus = nextCus->prev;
				}
				while(nextCus != this->X);

				do {
					if(currentCus->energy > 0) {
						currentCus->name = reverseList1->name;
						currentCus->energy = reverseList1->energy;
						reverseList1 = reverseList1->prev;
					}
					currentCus = currentCus->prev;
				}
				while(currentCus != this->X);

				do {
					if(nextCus->energy < 0) {
						customer* cus = new customer(nextCus->name, nextCus->energy, NULL, NULL);
						if(reverseList2 == NULL) reverseList2 = cus;
						else {
							reverseList2->next = cus;
							cus->prev = reverseList2;
							reverseList2 = cus;
						}
					}
					nextCus = nextCus->prev;
				}
				while(nextCus != this->X);

				do {
					if(currentCus->energy < 0) {
						currentCus->name = reverseList2->name;
						currentCus->energy = reverseList2->energy;
						reverseList2 = reverseList2->prev;
					}
					currentCus = currentCus->prev;
				}
				while(currentCus != this->X);
            }
			else {
				do {
					if(nextCus->energy < 0) {
						customer* cus = new customer(nextCus->name, nextCus->energy, NULL, NULL);
						if(reverseList1 == NULL) reverseList1 = cus;
						else {
							reverseList1->next = cus;
							cus->prev = reverseList1;
							reverseList1 = cus;
						}
					}
					nextCus = nextCus->prev;
				}
				while(nextCus != this->X);

				do {
					if(currentCus->energy < 0) {
						currentCus->name = reverseList1->name;
						currentCus->energy = reverseList1->energy;
						reverseList1 = reverseList1->prev;
					}
					currentCus = currentCus->prev;
				}
				while(currentCus != this->X);

				do {
					if(nextCus->energy > 0) {
						customer* cus = new customer(nextCus->name, nextCus->energy, NULL, NULL);
						if(reverseList2 == NULL) reverseList2 = cus;
						else {
							reverseList2->next = cus;
							cus->prev = reverseList2;
							reverseList2 = cus;
						}
					}
					nextCus = nextCus->prev;
				}
				while(nextCus != this->X);

				do {
					if(currentCus->energy > 0) {
						currentCus->name = reverseList2->name;
						currentCus->energy = reverseList2->energy;
						reverseList2 = reverseList2->prev;
					}
					currentCus = currentCus->prev;
				}
				while(currentCus != this->X);
			}

			//dat lai X
			currentCus = this->X;
			do {
				if(currentCus->name == nameX) {
					this->X = currentCus;
					break;
				}
				currentCus = currentCus->next;
			}
			while(currentCus != this->X);
		}

		//tính từ head của sublist, tính tổng energy của length phần tử
		int getSum(customer* head, int length) {
			if(head == NULL) return 0; //???????
			if(length == 0) return 0;
			int sum = 0;
			int i = 0;
			customer* temp = head;
			while(i < length) {
				sum += temp->energy;
				temp = temp->next;
				i++;
			}
			return sum;
		} 

		void UNLIMITED_VOID()
		{
			// cout << "unlimited_void" << endl;
			customer* longestList = NULL;	//dùng để giữ sublist thỏa yêu cầu để in ra
			customer* currentList = NULL;	//dùng để keep track, tìm ra sublist thỏa yêu cầu
			customer* cur = this->X;
			int longest = 0;
			int smallest = getSum(cur, 4); 	//tính min sum để so sánh, 
													//có cần ít nhất là 4 không nhỉ???? khi nào thì nó không tồn tại

			// int num = getSize(this->X);
			int num = 0;
			do {
				num++;
				cur = cur->next;
			}
			while(cur != this->X);

			if(num < 4) return;
			
			//tìm sublist dài nhất (>=4) có tổng energy nhỏ nhất 
			for(int i = 4; i <= num; i++) {
				cur = this->X;
				do {
					int tempSum = getSum(cur, i);
					if(tempSum <= smallest) {
						smallest = tempSum;
						if(longest <= i) longest = i;
						currentList = cur;
					}
					cur = cur->next;
				}
				while(cur != this->X);
			}

			//tới đây có list bắt đầu từ currentList, dài longest phần tử
			//clone nó qua longestList, có list dạng vòng
			customer* lastNode = NULL;
			int j = longest;
			while(j > 0) {
				customer* temp = new customer(currentList->name, currentList->energy, temp, temp);
				if(longestList == NULL) {
					longestList = temp;
					lastNode = temp;
				}
				else {
					temp->prev = lastNode;
					lastNode->next = temp;
					temp->next = longestList;
					longestList->prev = temp;
					lastNode = temp;					
				}
				currentList = currentList->next;
				j--;
			}

			//tìm phần tử nhỏ nhất trong sublist
			// longestList = longestList->next;
			customer* tmp = longestList;
			customer* hold = longestList;
			int small = tmp->energy;
			do {
				if(small > tmp->energy) {
					small = tmp->energy;
					hold = tmp;
				}
				tmp = tmp->next;
			}
			while(tmp != longestList);

			//in ra từ phần tử nhỏ nhất trong sublist
			tmp = hold;
			do {
				string out;
				out = hold->name + '-' + to_string(hold->energy) + '\n';
				cout << out;
				hold = hold->next;
			}
			while(hold != tmp);
		}

		//tính tổng energy trong 1 list
		//type=1 la cts, type=0 la oan linh
		int getEnergySum(customer* head, bool type, bool queue) {
			int sum = 0;
			customer* temp = head;
			if(type) {
				if(queue) {
					while(temp != NULL) {
						if(temp->energy > 0)
							sum += temp->energy;
						temp = temp->next;
					}
				}
				else {
					do {
						if(temp->energy > 0)
							sum += temp->energy;
						temp = temp->next;
					}
					while(temp != head);
				}
			}
			else {
				if(queue) {
					while(temp != NULL) {
						if(temp->energy < 0)
							sum += temp->energy;
						temp = temp->next;
					}
				}
				else {
					do {
						if(temp->energy < 0)
							sum += temp->energy;
						temp = temp->next;
					}
					while(temp != head);
				}
			}
			// delete temp;
			return sum;
		}

		//remove 1 customer from queue
		void remove(customer*& head, string name, int& count) {
			customer* temp = head;
			if(head == NULL) return;
			if(temp->name == name) {
				if(temp->next) {
					temp->next->prev = NULL;
				}
				head = temp->next;
				delete temp;
				count--;
				return;
			}

			while(temp && (temp->name != name)) temp = temp->next;
			if(!temp) return;

			if(temp->next) temp->next->prev = temp->prev;
			if(temp->prev) temp->prev->next = temp->next;

			delete temp;
			count--;
		}

		//remove 1 customer from generalList
		void removeGen(customer*& head, string name, int& count) {
			customer* temp = head;
			if(head == NULL) return;
			if(temp->name == name) {
				if(temp->prev) {
					temp->prev->prev = NULL;
				}
				head = temp->prev;
				delete temp;
				count--;
				return;
			}

			while(temp && (temp->name != name)) temp = temp->prev;
			if(!temp) return;

			if(temp->next) temp->next->prev = temp->prev;
			if(temp->prev) temp->prev->next = temp->next;

			// delete temp;
			count--;
		}

		void removeTable(customer* head, string name, int& count) {
			customer* temp = head;
			if(count == 1) {
				head->next = NULL;
				head->prev = NULL;
			}
			else {
				do {
					if(temp->name == name) {
						temp->prev->next = temp->next;
						temp->next->prev = temp->prev;
						delete temp;
						count--;
						return;
					}
					temp = temp->next;
				}
				while(temp != head);
			}
		}

		//type=1 la chu thuat su, type=0 la oan linh
		//remove all trong table
		void removeAllTable(customer*& head, bool type, int& count) {
			customer* temp = head;
			customer* current = head;
			do {
				if(type && current->energy > 0) {
					customer* toRemove = current;
					head = current->next;
					current = current->next;
					removeTable(head, toRemove->name, count);
				}
				else if(!type && current->energy < 0) {
					customer* toRemove = current;
					head = current->prev;
					current = current->next;
					removeTable(head, toRemove->name, count);
				}
				else current = current->next;
			}
			while(current != temp);
			//cap nhat lai vi tri cua X
		}

		//remove all trong queue
		void removeAllQueue(customer* head, bool type, int& count) {
			// customer* temp = head;
			if(head == NULL) return;
			customer* current = head;
			do {
				if(type && current->energy > 0) {
					customer* toRemove = current;
					if(toRemove == head)
						head = head->next;
					current = current->next;
					remove(head, toRemove->name, count);
				}
				else if(!type && current->energy < 0) {
					customer* toRemove = current;
					if(toRemove == head)
						head = head->next;
					current = current->next;
					remove(head, toRemove->name, count);
				}
				else current = current->next;
			}
			while(current != NULL);
		}

		//chú linh = chú thuật sư + oán linh
		void DOMAIN_EXPANSION()
		{
			// cout << "domain_expansion" << endl;
			int eSum1 = 0;		//tổng energy của oán linh, lấy trị tuyệt đối
			int eSum2 = 0;		//tổng energy của chú thuật sư

			eSum1 = abs(getEnergySum(this->X, 0, 0)) + abs(getEnergySum(this->frontQ, 0, 1));
			eSum2 = getEnergySum(this->X, 1, 0) + getEnergySum(this->frontQ, 1, 1);
			
			//đuổi oán linh trong table và queue
			if(eSum2 >= eSum1) {
				removeAllTable(this->X, 0, cusCount);
				removeAllQueue(this->frontQ, 0, waitCount);
			}
			//đuổi chú thuật sư trong table và queue
			else {
				removeAllTable(this->X, 1, cusCount);
				removeAllQueue(this->frontQ, 1, waitCount);
			}

			//nếu có chỗ trống trong table, xếp khách trong queue vào table
			customer* add = frontQ;
			while((cusCount < MAXSIZE) && (waitCount > 0)) {
				addFromQueue(add);
				add = add->next;
			}
			customer* temp = generalList;
			//cap nhat lai X, ng cuoi cung bi duoi la ng dau tien trong generalList
			customer* nextX = this->X;
			do {
				if(nextX->name == temp->name) {
					this->X = nextX;
					break;
				}
				nextX = nextX->next;
			}
			while(nextX != this->X);

			//in ra khách bị đuổi, từ đến trễ nhất tới đến sớm nhất
			// customer* temp = generalList;
			if(eSum2 >= eSum1) {
				do {
					customer* toRemove = new customer(temp->name, temp->energy, nullptr, nullptr);
					if(temp->energy < 0) {
						string out;
						out = temp->name + '-' + to_string(temp->energy) + '\n';
						cout << out;
						removeGen(generalList, toRemove->name, totalCount);
					}
					temp = temp->prev;
					// delete toRemove;
				}
				while(temp != NULL);
			}
			else {
				do {
					customer* toRemove = new customer(temp->name, temp->energy, nullptr, nullptr);
					if(temp->energy > 0) {
						string out;
						out = temp->name + '-' + to_string(temp->energy) + '\n';
						cout << out;
						removeGen(generalList, toRemove->name, totalCount);
					}
					temp = temp->prev;
					// delete toRemove;
				}
				while(temp != NULL);
			}
		}

		void LIGHT(int num)
		{
			// cout << "light " << num << endl;
			customer* temp0 = this->frontQ;
			customer* temp = this->X;
			if(num == 0) {
				int i = 0;
				while(i < waitCount) {
					string out;
					out = temp0->name + '-' + to_string(temp0->energy) + '\n';
					cout << out;
					i++;
					temp0 = temp0->next;
				}
			}
			else if(num > 0) {
				int i = 0;
				while(i < cusCount) {
					string out;
					out = temp->name + '-' + to_string(temp->energy) + '\n';
					cout << out;
					i++;
					temp = temp->next;
				}
			}
			else if(num < 0) {
				int i = 0;
				while(i < cusCount) {
					string out;
					out = temp->name + '-' + to_string(temp->energy) + '\n';
					cout << out;
					i++;
					temp = temp->prev;
				}
			}
			// delete temp0;
			// delete temp;
		}
};