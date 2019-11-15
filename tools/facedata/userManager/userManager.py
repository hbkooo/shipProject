#!/usr/bin/python3
# -*- coding: utf-8 -*-
import sqlite3
import uuid

user_table = ('userId', 'userName', 'userCompany',
              'telephone', 'email', 'companyAddress')

sql_create_user = """ CREATE TABLE IF NOT EXISTS user(
                            userId             CHAR(50)  PRIMARY KEY  NOT NULL,
                            userName           CHAR(50)    NOT NULL,
                            userCompany        CHAR(100),
                            telephone          CHAR(50),
                            email              CHAR(20),
                            companyAddress     CHAR(200)
                            );"""

class UserControl(object):
    '''base operation for user table of database.'''
    connection ="" 
    cursor = ""
    def __init__(self, database):
        self.connection = sqlite3.connect(str(database))
        self.cursor = self.connection.cursor()
        self.cursor.execute(sql_create_user)
        self.connection.commit()

    def add(self, values):
        self.cursor.execute("INSERT INTO user VALUES(?,?,?,?,?,?)", values)
        self.connection.commit()
    
    def delete(self, value):
        self.cursor.execute("DELETE FROM user WHERE userName = ?", (value,))
        check = input("Sure to delete %s's information? [y/N]" % value)
        if check is "y" or check is "Y":
            self.connection.commit()
        else :
             print("Delete cancelled!")

    def update(self, name, key, value):
        self.cursor.execute("UPDATE user SET %s = ? where userName = ?" % key, (value, name,))
        self.connection.commit()
        
    def select_by_user_name(self, value):
        return self.cursor.execute("SELECT * FROM user WHERE userName = ?" , (value,))

    def select_by_id(self, value):
        return self.cursor.execute("SELECT * FROM user WHERE userId = ?", (value,))

    def select_all(self):
        return self.cursor.execute("SELECT * FROM user")

    def close(self):
        self.cursor.close()
        self.connection.close()

def show_input_info(user_info):
    """
    show info that input before
    """
    print(" === Please check user infomation === ")
    for i in range(0, len(user_table)):
        print("[" + str(i) + "]" + user_table[i] + " : " + user_info[i])

def check_input_info(user_info):
    """modify user info input before"""
    # print user info
    show_input_info(user_info)
    is_correct = input("Is the user information above correct ? [Y/n]")
    if is_correct is "n" or is_correct is "N":
        num = input("Input number to modify information : ")
        modified_info = input("Input correct information : ")
        if (num.isnumeric()) and \
           (modified_info is not None) and \
           (int(num) > 0 and int(num) < 6):
            user_info[int(num)] = modified_info
            print("Modify successed.")
        else:
            print("Invalid input. Modify failed.")
            return -1
    return 0

def add_user(userControl):
    """
    input user info then write into DB
    """
    user_info = []
    # input
    user_info.append(str(uuid.uuid4()))
    print("Please input user infomation:")
    for item in user_table[1:]:
        user_info.append(input(item + ": "))
    # check
    while  check_input_info(user_info):
        pass
    # write DB
    userControl.add(user_info)

def update_user(userControl):
    user_name = input("Input the user's name that you want to update:")
    for i in range(2, len(user_table)):
        print("[" + str(i) + "] " + user_table[i])
    user_info_key = input("Input the user's info item that you want to update [2-5]:")
    user_info_key = user_table[int(user_info_key)]
    user_info_value = input("Input the user's info value of %s that you want to update:" % user_info_key)
    userControl.update(user_name, user_info_key, user_info_value)
    
def delete_user(userControl):
    user_name = input("Input the user's name that you want to delete:")
    userControl.delete(user_name)

def show_user(userControl):
    print("[0] userId")
    print("[1] userName")
    key = input("Input the item you want use to search [0/1]:")
    key = user_table[int(key)]
    user_item = input("Input the user's %s that you want to search:" % key)
    if str(key) == "userId":
        rows = userControl.select_by_id(user_item)
    elif str(key) == "userName":
        rows = userControl.select_by_user_name(user_item)
    else:
        print("Invalid input : " + key)
        return 0
    for row in rows:
            print(row)

def show_all(userControl):
    for row in userControl.select_all():
        print(row)

def helper():
    """show help info"""
    help_info = "=========================================================\n" +\
                "Press numbers to select function: \n" +\
                "[1] Add information of a new user to database. \n" +\
                "[2] Update information of an exist user . \n" +\
                "[3] Delete information of an exist user of the database. \n" +\
                "[4] Search information of a exist user of the database. \n" +\
                "[5] Show information of a all user of the database. \n" +\
                "[0] Quit. \n"+\
                "=========================================================\n"
    return input(help_info)

def main():
    """main"""
    userControl = UserControl("user.db")
    while True:
        func = helper()
        if func is "1":
            add_user(userControl)
        elif func is "2":
            update_user(userControl)
        elif func is "3":
            delete_user(userControl)
        elif func is "4":
            show_user(userControl)
        elif func is "5":
            show_all(userControl)
        elif func is "0":
            break
        else:
            print("Input Error!")
    userControl.close()
    
if __name__ == '__main__':
    main()
