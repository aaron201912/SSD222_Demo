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
	ZKDiagram(HWND hParentWnd);
	virtual ~ZKDiagram();

	/**
	 * @brief 设置波形线宽
	 * @param index 波形索引值
	 * @param width 波形线宽
	 */
	void setPenWidth(int index, int width);

	/**
	 * @brief 设置波形颜色
	 * @param index 波形索引值
	 * @param color 波形颜色值：0x ARGB
	 */
	void setPenColor(int index, ARGB color);

	/**
	 * @brief 设置波形X轴方向缩放比例
	 * @param index 波形索引值
	 * @param xScale X轴方向缩放比例
	 */
	void setXScale(int index, double xScale);

	/**
	 * @brief 设置波形Y轴方向缩放比例
	 * @param index 波形索引值
	 * @param yScale Y轴方向缩放比例
	 */
	void setYScale(int index, double yScale);

	/**
	 * @brief 设置波形数据
	 * @param index 波形索引值
	 * @param pPoints 坐标数组
	 * @param count 数组长度
	 */
	void setData(int index, const MPPOINT *pPoints, int count);

	/**
	 * @brief 添加波形数据
	 * @param index 波形索引值
	 * @param data Y轴方向数据值
	 */
	void addData(int index, float data);

	void addDiagramInfo(int width, ARGB color, EDiagramStyle style,
			double xScale, double yScale, float step, UINT eraseSpace, bool isAntialiasOn);

protected:
	ZKDiagram(HWND hParentWnd, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_DIAGRAM; }

	virtual void onDraw(HDC hdc);

	void _section_(zk) drawDiagram(HDC hdc);

private:
	void _section_(zk) parseDiagramAttributeFromJson(const Json::Value &json);

private:
	typedef struct {
		float lower;
		float upper;
	} SAxisRange;

	typedef struct {
		HGRAPHICS graphics;
		HPATH path;
		HPEN pen;
		int penWidth;
		ARGB penColor;
		EDiagramStyle style;
		double xScale;
		double yScale;
		MPPOINT bufPoints[8];
		int bufIndex;
		float step;
		UINT eraseSpace;
		MPMatrix matrix;
	} SDiagramInfo;

	void updateMatrix(SDiagramInfo &info);
	void transform(const MPMatrix &matrix, float &x, float &y) const;

private:
	vector<SDiagramInfo *> mDiagramInfoList;

	LayoutPosition mRegionPosition;		// 曲线图绘制区域

	SAxisRange mXAxisRange;		// X轴坐标范围
	SAxisRange mYAxisRange;		// Y轴坐标范围

	double mXBaseScale;
	double mYBaseScale;
};

#endif /* _CONTROL_ZKDIAGRAM_H_ */
