#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int phone_number;
  char [] name;
  Contact *next;

}Contact

Contact root = NULL;

int add_person(char[] name, int phone_number){
  Contact new, temp;
  Contact new = malloc(sizeof(Contact));1
  new.name = name;
  new.phone_number = phone_number;
  if (root == Null){
    root = new;
    root.next = NULL;
    return 0;
  }
  temp = root;
  while(root.next != NULL){

  }


}


int main(){




  return 0;
}
