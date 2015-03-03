#include "stdafx.h"
#include "MCamera.h"
#include <algorithm>
PointInfo::PointInfo(void )
{
	eye = Vector3(0, 0, 0);
	center = Vector3(0.0, 0.0, 0.0);
	up = Vector3(0.0, 1.0, 0.0);
	cf = 100;
	PointType = NORMAL_POINT;
	linkNum = 0;
	linkTo = 0;
}
PointInfo::PointInfo(const PointInfo &point)
{
	eye = point.eye;
	center = point.center;
	up = point.up;
	cf = point.cf;
	PointType = point.PointType;
	linkNum = point.linkNum;
	linkTo = point.linkTo;
}
PointInfo PointInfo:: operator = (const PointInfo & p)
{
	eye = p.eye;
	center = p.center;
	up = p.up;
	cf = p.cf;
	PointType = p.PointType;
	linkNum = p.linkNum;
	linkTo = p.linkTo;
	return *this;
}
bool PointInfo::operator == (const PointInfo & p)
{
	if (eye == p.eye && up == p.up && center == p.center)
	{
		return true;

	}
	else{
		return false;
	}

}
void MCamera::UpdateUndoStack(unsigned int pos)
{
	BackUpPointList(pos);
	undoStack.push(_backup);
}

void MCamera::UpdateUndoStackAndClearRedoStack(unsigned int pos)
{
	BackUpPointList(pos);
	undoStack.push(_backup);
	stack< StackInfo > empty2;
	std::swap(redoStack, empty2);
}
void MCamera::UpdateRedoStack(unsigned int pos)
{
	BackUpPointList(pos);
	redoStack.push(_backup);
}
void MCamera::BackUpPointList(unsigned int pos)
{
	_backup.pos = pos;
	_backup.bak.clear();
	if (pos > PointList.size())
	{
		return;
	}
	vector<PointInfo>::iterator pre_it = PointList.begin() + pos;
	for (; pre_it != PointList.end(); pre_it++)
	{
		_backup.bak.push_back(*pre_it);
	}
}
void MCamera::reloadBackUp(StackInfo & backUp)
{
	PointList.erase(PointList.begin() + backUp.pos,
		PointList.end());
	vector<PointInfo>::iterator it = backUp.bak.begin();
	for (; it != backUp.bak.end(); it++)
	{
		PointList.push_back(*it);
	}

}
int MCamera::undo()
{
	if (undoStack.empty())
	{
		return -1;
	}
	StackInfo &backUp = undoStack.top();
	int pos = backUp.pos;
	UpdateRedoStack(pos);
	reloadBackUp(backUp);

	undoStack.pop();
	if (pos > PointList.size() -1)
		return PointList.size() -1;

	return pos;
}
int MCamera::redo()
{
	if (redoStack.empty())
		return -1;
	StackInfo &backUp = redoStack.top();
	int pos = backUp.pos;
	UpdateUndoStack(pos);
	reloadBackUp(backUp);

	redoStack.pop();
	if (pos > PointList.size() -1)
		return PointList.size() -1;
	return pos;
}
void MCamera::AddPoint(unsigned int pos, const PointInfo & point)
{
	// 在undo stack 中备份pos之后的所有点信息

	UpdateUndoStackAndClearRedoStack(pos);
	if (pos < PointList.size())
		PointList.insert( PointList.begin() + pos, point);
	else
		if (pos == PointList.size())
			PointList.push_back(point);
	// set pointList[1]
	PointList[1].cf = 0;
	vector<PointInfo>::iterator it = PointList.begin() + pos + 1;
	if (it != PointList.end() && point.PointType != STOP_POINT &&
		it->PointType != NORMAL_POINT && it->PointType != LINK_POINT
		&& it->PointType != point.PointType)
	{
		it->PointType = NORMAL_POINT;
		Ui_ShowInfo("警告：在此处加入关键点，将导致之后关键点退化为普通关键点！");
	}
	// update linkto;
	for (; it != PointList.end(); it++)
	{
		if(it->PointType == LINK_POINT && it->linkTo >= pos)
			it->linkTo ++;
	}
	// Redo stack

}
void MCamera::DeletePoint(unsigned int pos)
{
	// 在undo stack 中备份pos之后的所有点信息
	UpdateUndoStackAndClearRedoStack(pos);
	vector<PointInfo>::iterator it = PointList.begin() + pos + 1;
	vector<PointInfo>::iterator pre = PointList.begin() + pos ;
	if (it != PointList.end() && pre->PointType != STOP_POINT &&
		it->PointType != NORMAL_POINT && it->PointType != LINK_POINT
		&& it->PointType != pre->PointType)
	{
		it->PointType = NORMAL_POINT;
		Ui_ShowInfo("警告：删除此点后，将导致之后关键点退化为普通关键点！");
	}
	// update linkto;
	for (; it != PointList.end(); it++)
	{
		if(it->PointType == LINK_POINT)
		{
			if( it->linkTo > pos)
				it->linkTo --;
			else  if (it->linkTo == pos)
			{
				it->linkTo = 0;
				it->PointType = NORMAL_POINT;
				Ui_ShowInfo("警告：删除此点后，将导致链接到该点的Link点退化为普通点");
			}
		}
	}
	//delete
	if (pos < PointList.size())
		PointList.erase(PointList.begin() + pos);

	if (PointList.size() > 2)
		PointList[1].cf = 0;
}
const PointInfo& MCamera::GetPointDetail(unsigned int pos)
{
	vector<PointInfo>::iterator pitor;
	if (pos < PointList.size())
	{
		pitor = PointList.begin() + pos;
		return *pitor;
	}
	return *(PointList.begin());
}
void MCamera::ChangePointInfo(unsigned int pos, const PointInfo & point)
{
	// 在undo stack 中备份pos之后的所有点信息
	UpdateUndoStackAndClearRedoStack(pos);
	unsigned int t;
	vector<PointInfo>::iterator pitor;
	if (pos < PointList.size())
	{
		// 改变位置参数
		UpdatePointList(pos, point);
	}
}
unsigned int MCamera::GetFrameId(unsigned int pos)
{
	unsigned int  f = 0;
	vector<PointInfo>::iterator it;
	int i;
	for (i = 1, it = PointList.begin() + 1; i <= pos 
		&&it != PointList.end(); it++, i++)
		f += it->cf;
	return f;
}
void MCamera::addLink(unsigned int pos)
{
	PointList[pos].linkNum ++;
}
void MCamera::deleteLink(unsigned int pos)
{
	PointList[pos].linkNum --;
}
vector<PointInfo>::size_type MCamera::GetPointsNum(void)
{
	return PointList.size() - 1;
}
void MCamera::UpdatePointList(unsigned int pos, const PointInfo & p)
{
	if (pos < 1 || pos >= PointList.size())
		return;
	PointInfo pre;		// previous point 修改前的值
	GLdouble len;
	Vector3 A;
	int linkNum = 0;
	BOOL UpdateNext;
	vector<PointInfo>::iterator it;
	vector<int> linkPointList;
	it = PointList.begin() + pos;
	pre = *it;
	/* update pos */
	*it = p;
	if (it->linkNum > 0)
	{
		linkNum += it->linkNum;
		linkPointList.push_back(pos);
	}
	UpdateNext = TRUE;
	// update	// 
	for (it ++, pos++; it != PointList.end() && (UpdateNext == TRUE || linkNum > 0); it++, pos++)
	{
		if (it->PointType == NORMAL_POINT)
		{
			UpdateNext = FALSE;
		}
		else if (it->PointType == LINK_POINT)
		{
			if (linkPointList.end() != std::find(linkPointList.begin(), linkPointList.end(),
				it->linkTo))
			{
				if (it->linkNum > 0)
				{
					linkPointList.push_back(pos);
					linkNum += it->linkNum;
				}
				linkNum --;
				UpdateNext = TRUE;
				pre = *it;
				it->center = PointList[it->linkTo].center;
				it->eye = PointList[it->linkTo].eye;
				it->up = PointList[it->linkTo].up;
			}
			else
				UpdateNext = FALSE;
			
		}
		if (UpdateNext == TRUE)
		{
			if (it->linkNum > 0)
			{
				linkPointList.push_back(pos);
				linkNum += it->linkNum;
			}
			switch(it->PointType)
			{
			case ZOOM_POINT:
				len = (it->eye - pre.eye).length();
				// 保持与之前相同的长度
				if ((pre.center - pre.eye).dotProduct(it->eye - pre.eye) < 0 )
					len *= -1;
				pre = *it;
				it->eye = (it - 1)->eye + ((it - 1)->center - (it - 1)->eye).normalize() * len;
				it->center = (it - 1)->center + ((it - 1)->center - (it - 1)->eye).normalize() * len;
				break;
			case PARALLEL_VIEW_POINT:
				pre = *it;
				it->center = (it - 1)->center + it->eye - (it - 1)->eye;
				break;
			case ROTATE_VIEW_POINT:
				A = (it - 1)->eye - pre.eye;
				pre = *it;
				it->eye = (it - 1)->eye;
				it->center = it->center + A;
				break;
			case STOP_POINT:
				it->center = (it - 1)->center;
				it->eye = (it - 1)->eye;
				it->up = (it - 1)->up;
				break;
			}
		}
	}

}