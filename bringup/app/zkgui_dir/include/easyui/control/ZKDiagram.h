/*
 * ZKDiagram.h
 *
 *  Created on: Jul 31, 2017
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKDIAGRAM_H_
#define _CONTROL_ZKDIAGRAM_H_

#include <vector>
#include "ZKBase.h"

class ZKDiagramPrivate;

/**
 * @brief 波形图控件
 */
class ZKDiagram : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKDiagram)

public:
	typedef enum {
		E_DIAGRAM_STYLE_LINE,		// 折线
		E_DIAGRAM_STYLE_CURVE		// 曲线
	} EDiagramStyle;

public:
	ZKDiagram(ZKBase *pParent);
	virtual ~ZKDiagram();

	/**
	 * @brief 设置波形线宽
	 * @param index 波形索引值
	 * @param width 波形线宽
	 */
	void setPenWidth(int index, uint32_t width);

	/**
	 * @brief 设置波形颜色
	 * @param index 波形索引值
	 * @param color 波形颜色值：0x RGB
	 */
	void setPenColor(int index, uint32_t color);

	/**
	 * @brief 设置波形X轴方向缩放比例
	 * @param index 波形索引值
	 * @param xScale X轴方向缩放比例
	 */
	void setXScale(int index, float xScale);

	/**
	 * @brief 设置波形Y轴方向缩放比例
	 * @param index 波形索引值
	 * @param yScale Y轴方向缩放比例
	 */
	void setYScale(int index, float yScale);

	/**
	 * @brief 设置波形数据
	 * @param index 波形索引值
	 * @param pPoints 坐标数组
	 * @param count 数组长度
	 */
	void setData(int index, const SZKPoint *pPoints, int count);

	/**
	 * @brief 添加波形数据
	 * @param index 波形索引值
	 * @param data Y轴方向数据值
	 */
	void addData(int index, float data);

	/**
	 * @brief 清除波形图
	 * @param index 波形索引值
	 */
	void clear(int index);

	void addDiagramInfo(uint32_t width, uint32_t color, EDiagramStyle style,
			float xScale, float yScale, float step, uint32_t eraseSpace, bool isAntialiasOn);

protected:
	ZKDiagram(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_DIAGRAM; }

	virtual void onDraw(ZKCanvas *pCanvas);

private:
	void _section_(zk) parseDiagramAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKDIAGRAM_H_ */
