#include "Table.h"
#include "Error.h"
#include <fstream>
#include <algorithm>

using namespace std;
extern ofstream fout;

int Table::num = 0;	//过程块(函数)编号

Table::Table() {
	level = 0;
}


/*查全局或数据表, 标识符是否重复定义*/
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


/*查全局表, 标识符是否重复定义*/
bool Table::is_found_global(string name) {
	for (iter = global.begin(); iter != global.end(); iter++) {
		if (iter->name == name) {
			return true;
		}
	}
	return false;
}


/*查全局和数据表, 返回记录  未找到返回addr为-1的记录*/
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


/*查全局表, 返回记录  未找到返回addr为-1的记录*/
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


/*全局表中最后一个函数记录*/
record Table::last_global() {
	return global.back();
}


/*/查找参数*/
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


/*在数据表添加*/
void Table::add(record r) {
	if (level == 0) {
		add_global(r);
		return;
	}
	r.level = level;
	t.push_back(r);
}


/*在全局表添加*/
void Table::add_global(record r) {
	r.level = level;
	global.push_back(r);
}

///////////////////////////////////////////////////////////////////////////////////

/*回填value*/
void Table::write_value(int value) {
	global.back().value = value;
}

/*回填addr*/
void Table::write_addr(int addr) {
	global.back().addr = addr;
}

/*函数编号num+1, 并赋值给当前level*/
void Table::new_level() {
	num++;
	level = num;
}
