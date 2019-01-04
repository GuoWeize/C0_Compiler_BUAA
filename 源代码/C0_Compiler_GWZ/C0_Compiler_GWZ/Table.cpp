#include "Table.h"
#include "Error.h"
#include <fstream>
#include <algorithm>

using namespace std;
extern ofstream fout;

int Table::num = 0;	//���̿�(����)���

Table::Table() {
	level = 0;
}


/*��ȫ�ֻ����ݱ�, ��ʶ���Ƿ��ظ�����*/
bool Table::is_found(string name) {
	if (level == 0) {
		return is_found_global(name);
	}
	else {
		for (iter = t.begin(); iter != t.end(); iter++) {
			if (iter->level < level) {
				continue;
			}
			else if (iter->level > level) {
				break;
			}
			else {
				if (iter->name == name) {
					return true;
				}
			}
		}
	}
	return false;
}


/*��ȫ�ֱ�, ��ʶ���Ƿ��ظ�����*/
bool Table::is_found_global(string name) {
	for (iter = global.begin(); iter != global.end(); iter++) {
		if (iter->name == name) {
			return true;
		}
	}
	return false;
}


/*��ȫ�ֺ����ݱ�, ���ؼ�¼  δ�ҵ�����addrΪ-1�ļ�¼*/
record Table::find(string name) {
	for (iter = t.begin(); iter != t.end(); iter++) {
		if (iter->level < level) {
			continue;
		}
		else if (iter->level > level) {
			break;
		}
		else {
			if (iter->name == name) {
				return *iter;
			}
		}
	}
	for (iter = global.begin(); iter != global.end(); iter++) {
		if (iter->name == name) {
			return *iter;
		}
	}
	
	record rec;
	rec.addr = -1;
	return rec;
}


/*��ȫ�ֱ�, ���ؼ�¼  δ�ҵ�����addrΪ-1�ļ�¼*/
record Table::find_global(string name) {
	for (iter = global.begin(); iter != global.end(); iter++) {
		if (iter->name == name) {
			return *iter;
		}
	}

	record rec;
	rec.addr = -1;
	return rec;
}


/*ȫ�ֱ������һ��������¼*/
record Table::last_global() {
	return global.back();
}


/*/���Ҳ���*/
record Table::find_para(int func_level, int para_index) {
	for (iter = t.begin(); iter != t.end(); iter++) {
		if (iter->level < func_level) {
			continue;
		}
		else if (iter->level > func_level) {
			break;
		}
		else {
			if (para_index == iter->value) {
				return *iter;
			}
		}
	}
	record rec;
	rec.addr = -1;
	return rec;
}


/*�����ݱ����*/
void Table::add(record r) {
	if (level == 0) {
		add_global(r);
		return;
	}
	r.level = level;
	t.push_back(r);
}


/*��ȫ�ֱ����*/
void Table::add_global(record r) {
	r.level = level;
	global.push_back(r);
}

///////////////////////////////////////////////////////////////////////////////////

/*����value*/
void Table::write_value(int value) {
	global.back().value = value;
}

/*����addr*/
void Table::write_addr(int addr) {
	global.back().addr = addr;
}

/*�������num+1, ����ֵ����ǰlevel*/
void Table::new_level() {
	num++;
	level = num;
}
