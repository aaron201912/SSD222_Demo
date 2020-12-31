/*
 * ZKListView.h
 *
 *  Created on: Jun 28, 2017
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKLISTVIEW_H_
#define _CONTROL_ZKLISTVIEW_H_

#include "ZKButton.h"

class ZKListViewPrivate;
class ZKListSubItemPrivate;

/**
 * @brief 列表控件
 */
class ZKListView : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKListView)

public:
	ZKListView(ZKBase *pParent);
	virtual ~ZKListView();

public:
	class ZKListItem;
	class ZKListSubItem : public ZKButton {
		friend class ZKListView;
		friend class ZKListItem;
		ZK_DECLARE_PRIVATE(ZKListSubItem)
	public:
		ZKListSubItem();
		virtual ~ZKListSubItem();

	protected:
		virtual bool createWindow() { return false; }
		virtual const char* getClassName() const { return NULL; }

		void drawSubItem(ZKCanvas *pCanvas, int xOffset, int yOffset);

		int getLongClickTimeOut() const;
		int getLongClickIntervalTime() const;
	};

	class ZKListItem : public ZKListSubItem {
		friend class ZKListView;
		friend class ZKListViewPrivate;
	public:
		ZKListItem();
		virtual ~ZKListItem();

		/**
		 * @brief 通过ID值获取子项
		 */
		ZKListSubItem* findSubItemByID(int id) const;
		int getSubItemCount() const { return mSubItemCount; }

	protected:
		virtual void onBeforeCreateWindow(const Json::Value &json);

		int hitItemID(const MotionEvent &ev);
		void drawItem(ZKCanvas *pCanvas, int xOffset, int yOffset);
		void setItemPressed(int itemID, bool isPressed);

	private:
		void parseListItemAttributeFromJson(const Json::Value &json);

	private:
		ZKListSubItem **mListSubItemList;
		int mSubItemCount;
	};

	/**
	 * @brief 数据与UI绑定适配器
	 */
	class AbsListAdapter {
		friend class ZKListView;
	public:
		virtual ~AbsListAdapter() { }
		virtual int getListItemCount(const ZKListView *pListView) const = 0;
		virtual void obtainListItemData(ZKListView *pListView, ZKListItem *pListItem, int index) = 0;

		/**
		 * 数据更新，重绘UI
		 */
		void notifyDataSetChanged(ZKListView *pListView) {
			if (pListView) {
				pListView->refreshListView();
			}
		}
	};

	/**
	 * @brief 列表项点击监听接口
	 */
	class IItemClickListener {
	public:
		virtual ~IItemClickListener() { }
		virtual void onItemClick(ZKListView *pListView, int index, int itemID) = 0;
	};

	/**
	 * @brief 列表项长按监听接口
	 */
	class IItemLongClickListener {
	public:
		virtual ~IItemLongClickListener() { }
		virtual void onItemLongClick(ZKListView *pListView, int index, int itemID) = 0;
	};

	void setListAdapter(AbsListAdapter *pAdapter);
	void setItemClickListener(IItemClickListener *pListener);
	void setItemLongClickListener(IItemLongClickListener *pListener);

	/**
	 * @brief 刷新列表
	 */
	void refreshListView();

	/**
	 * @brief 跳转到指定行或列
	 */
	void setSelection(int index);

	/**
	 * @brief 获取列表项宽
	 */
	uint32_t getItemWidth() const;

	/**
	 * @brief 获取列表项高
	 */
	uint32_t getItemHeight() const;

	/**
	 * @brief 获取列表行数
	 */
	uint32_t getRows() const;

	/**
	 * @brief 获取列表列数
	 */
	uint32_t getCols() const;

	/**
	 * @brief 获取列表总项数
	 */
	int getListItemCount() const;

	/**
	 * @brief 获取第一个可见项的索引值
	 */
	int getFirstVisibleItemIndex() const;

	/**
	 * @brief 获取第一个可见项的偏移值
	 */
	int getFirstVisibleItemOffset() const;

	/**
	 * @brief 设置惯性滑动减速比例
	 * @param ratio 减速比例 范围： 0 < ratio < 1
	 */
	void setDecRatio(float ratio);

protected:
	ZKListView(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_LISTVIEW; }

	virtual void onDraw(ZKCanvas *pCanvas);
	virtual bool onTouchEvent(const MotionEvent &ev);
	virtual void onTimer(int id);

private:
	void parseListViewAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKLISTVIEW_H_ */
