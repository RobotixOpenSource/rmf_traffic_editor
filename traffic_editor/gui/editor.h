/*
 * Copyright (C) 2019 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#ifndef EDITOR_H
#define EDITOR_H

#include <map>
#include <string>
#include <vector>

#include <QGraphicsItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsScene>
#include <QMainWindow>
#include <QSettings>

#include "project.h"
#include "editor_model.h"

class BuildingLevelTable;
class MapView;
class Level;
class LiftTable;
class ScenarioTable;
class TrafficTable;

QT_BEGIN_NAMESPACE
class QAction;
class QButtonGroup;
class QComboBox;
class QGraphicsView;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QListWidget;
class QMenu;
class QMouseEvent;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;
class QTabWidget;
class QToolButton;
QT_END_NAMESPACE


class Editor : public QMainWindow
{
  Q_OBJECT

public:
  Editor();

  static Editor *get_instance();

  /// Load a project, replacing the current project being edited
  bool load_project(const QString &filename);

  /// Attempt to load the most recently saved project, just for convenience
  /// when starting the application since often we want to 'resume' editing.
  bool load_previous_project();

  /// Attempt to restore the previous viewport scale and center point
  void restore_previous_viewport();

protected:
  void mousePressEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void keyPressEvent(QKeyEvent *event);
  void closeEvent(QCloseEvent *event);

private:
  enum ModeId
  {
    MODE_BUILDING = 1,
    MODE_TRAFFIC = 2,
    MODE_SCENARIO = 3
  } mode = MODE_BUILDING;

  void set_mode(const ModeId _mode, const QString& mode_string);

  enum ToolId
  {
    TOOL_SELECT = 1,
    TOOL_ADD_VERTEX,
    TOOL_MOVE,
    TOOL_ADD_LANE,
    TOOL_ADD_WALL,
    TOOL_ADD_MEAS,
    TOOL_ADD_DOOR,
    TOOL_ADD_MODEL,
    TOOL_ROTATE,
    TOOL_ADD_FLOOR,
    TOOL_EDIT_POLYGON,
    TOOL_ADD_ZONE,
    TOOL_ADD_FIDUCIAL,
    TOOL_ADD_ROI
  } tool_id = TOOL_SELECT;

  std::map<ToolId, QAction *> tools;
  void set_tool_visibility(const ToolId id, const bool visible);

  /////////////////
  // MENU ACTIONS
  void project_new();
  void project_open();
  bool project_save();

  bool maybe_save();
  void edit_preferences();
  void edit_building_properties();
  void edit_project_properties();

  void level_add();
  void level_edit();
  void update_level_buttons();

  void zoom_fit();

  void help_about();


  bool is_mouse_event_in_map(QMouseEvent *e, QPointF &p_scene);

  QToolBar *toolbar;
  QToolButton *create_tool_button(
      const ToolId id,
      const QString& icon_filename,
      const QString &tooltip);
  void tool_toggled(int id, bool checked);

  QComboBox *mode_combo_box;

/////////////////////////////
  static Editor *instance;  // there will only be one instance

  Project project;
  int level_idx = 0;  // level that we are currently editing
  int clicked_idx = -1;  // point most recently clicked
  int polygon_idx = -1;  // currently selected polygon

  QButtonGroup *level_button_group;
  QHBoxLayout *level_button_hbox_layout;
  QGraphicsScene *scene;
  MapView *map_view;

  QAction *save_action;
  QAction *zoom_in_action, *zoom_out_action;
  QAction *zoom_normal_action, *zoom_fit_action;

  const QString tool_id_to_string(const int id);
  QButtonGroup *tool_button_group;

  QTabWidget *right_tab_widget;

  QTableWidget *create_tabbed_table();

  QTableWidget *layers_table;
  void populate_layers_table();
  void layers_table_set_row(
      const int row_idx,
      const QString &label,
      const bool checked);
  void layer_edit_button_clicked(const std::string &label);
  void layer_add_button_clicked();

  BuildingLevelTable *level_table;
  LiftTable *lift_table;
  ScenarioTable *scenario_table;
  TrafficTable *traffic_table;

  QTableWidget *property_editor;
  void update_property_editor();
  void clear_property_editor();
  void populate_property_editor(const Edge& edge);
  void populate_property_editor(const Model& model);
  void populate_property_editor(const Vertex& vertex);
  void populate_property_editor(const Fiducial& fiducial);

  QTableWidgetItem *create_table_item(const QString &str, bool editable=false);
  void property_editor_cell_changed(int row, int column);
  void property_editor_set_row(
      const int row_idx,
      const QString &label,
      const QString &value,
      const bool editable = false);
  void property_editor_set_row(
      const int row_idx,
      const QString &label,
      const int &value,
      const bool editable = false);
  void property_editor_set_row(
      const int row_idx,
      const QString &label,
      const double &value,
      const int max_decimal_places = 3,
      const bool editable = false);
  QPushButton *add_param_button, *delete_param_button;
  void add_param_button_clicked();
  void delete_param_button_clicked();

  std::vector<EditorModel> editor_models;
  EditorModel *mouse_motion_editor_model = nullptr;
  void load_model_names();

  int get_polygon_idx(const double x, const double y);

  bool create_scene();

  const static int ROTATION_INDICATOR_RADIUS = 50;
  QGraphicsLineItem *mouse_motion_line = nullptr;
  QGraphicsEllipseItem *mouse_motion_ellipse = nullptr;
  QGraphicsPixmapItem *mouse_motion_model = nullptr;
  QGraphicsPolygonItem *mouse_motion_polygon = nullptr;

  int mouse_model_idx = -1;
  int mouse_vertex_idx = -1;
  int mouse_fiducial_idx = -1;
  std::vector<int> mouse_motion_polygon_vertices;
  int mouse_motion_polygon_vertex_idx = -1;

  void draw_mouse_motion_line_item(const double mouse_x, const double mouse_y);
  void remove_mouse_motion_item();
  void set_selected_line_item(QGraphicsLineItem *line_item);

  bool line_vertices_match(
      const QGraphicsLineItem *line_item,
      const Vertex &v1,
      const Vertex &v2,
      const double max_dist);

  void number_key_pressed(const int n);

  // mouse handlers for various tools
  enum MouseType {
    MOUSE_UNDEFINED = 0,
    MOUSE_PRESS = 1,
    MOUSE_RELEASE = 2,
    MOUSE_MOVE = 3
  };

  void mouse_event(const MouseType t, QMouseEvent *e);

  // helper function to avoid repeating lots of "add edge" code
  void mouse_add_edge(
      const MouseType t,
      QMouseEvent *e,
      const QPointF &p,
      const Edge::Type &edge_type);

  // helper function to avoid repeating lots of "add polygon" code
  void mouse_add_polygon(
      const MouseType t,
      QMouseEvent *e,
      const QPointF &p,
      const Polygon::Type &polygon_type);

  QGraphicsPixmapItem *get_closest_pixmap_item(const QPointF &p);
  double discretize_angle(const double &angle);

  void mouse_select(const MouseType t, QMouseEvent *e, const QPointF &p);
  void mouse_move(const MouseType t, QMouseEvent *e, const QPointF &p);
  void mouse_rotate(const MouseType t, QMouseEvent *e, const QPointF &p);

  void mouse_add_vertex(const MouseType t, QMouseEvent *e, const QPointF &p);
  void mouse_add_fiducial(const MouseType t, QMouseEvent *e, const QPointF &p);
  void mouse_add_lane(const MouseType t, QMouseEvent *e, const QPointF &p);
  void mouse_add_wall(const MouseType t, QMouseEvent *e, const QPointF &p);
  void mouse_add_meas(const MouseType t, QMouseEvent *e, const QPointF &p);
  void mouse_add_door(const MouseType t, QMouseEvent *e, const QPointF &p);
  void mouse_add_model(const MouseType t, QMouseEvent *e, const QPointF &p);
  void mouse_add_floor(const MouseType t, QMouseEvent *e, const QPointF &p);
  void mouse_add_roi(const MouseType t, QMouseEvent *e, const QPointF &p);
  void mouse_edit_polygon(const MouseType t, QMouseEvent *e, const QPointF &p);

  QPointF previous_mouse_point;
};

#endif