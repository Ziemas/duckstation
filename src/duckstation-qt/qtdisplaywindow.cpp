#include "qtdisplaywindow.h"
#include "imgui.h"
#include "qthostinterface.h"
#include "qtutils.h"
#include <QtGui/QKeyEvent>

QtDisplayWindow::QtDisplayWindow(QtHostInterface* host_interface, QWindow* parent)
  : QWindow(parent), m_host_interface(host_interface)
{
}

QtDisplayWindow::~QtDisplayWindow() = default;

HostDisplay* QtDisplayWindow::getHostDisplayInterface()
{
  return nullptr;
}

bool QtDisplayWindow::hasDeviceContext() const
{
  return true;
}

bool QtDisplayWindow::createDeviceContext(QThread* worker_thread, bool debug_device)
{
  return true;
}

bool QtDisplayWindow::initializeDeviceContext(bool debug_device)
{
  if (!createImGuiContext() || !createDeviceResources())
    return false;

  return true;
}

void QtDisplayWindow::destroyDeviceContext()
{
  destroyImGuiContext();
  destroyDeviceResources();
}

bool QtDisplayWindow::createImGuiContext()
{
  ImGui::CreateContext();

  auto& io = ImGui::GetIO();
  io.IniFilename = nullptr;
  io.DisplaySize.x = static_cast<float>(getScaledWindowWidth());
  io.DisplaySize.y = static_cast<float>(getScaledWindowHeight());

  const float framebuffer_scale = static_cast<float>(devicePixelRatio());
  io.DisplayFramebufferScale.x = framebuffer_scale;
  io.DisplayFramebufferScale.y = framebuffer_scale;
  io.FontGlobalScale = framebuffer_scale;
  ImGui::GetStyle().ScaleAllSizes(framebuffer_scale);

  return true;
}

void QtDisplayWindow::destroyImGuiContext()
{
  ImGui::DestroyContext();
}

bool QtDisplayWindow::createDeviceResources()
{
  return true;
}

void QtDisplayWindow::destroyDeviceResources() {}

void QtDisplayWindow::WindowResized(s32 new_window_width, s32 new_window_height)
{
  // imgui may not have been initialized yet
  if (!ImGui::GetCurrentContext())
    return;

  auto& io = ImGui::GetIO();
  io.DisplaySize.x = static_cast<float>(new_window_width);
  io.DisplaySize.y = static_cast<float>(new_window_height);
}

void QtDisplayWindow::keyPressEvent(QKeyEvent* event)
{
  if (event->isAutoRepeat())
    return;

  m_host_interface->handleKeyEvent(QtUtils::KeyEventToInt(event), true);
}

void QtDisplayWindow::keyReleaseEvent(QKeyEvent* event)
{
  if (event->isAutoRepeat())
    return;

  m_host_interface->handleKeyEvent(QtUtils::KeyEventToInt(event), false);
}

void QtDisplayWindow::resizeEvent(QResizeEvent* event)
{
  QWindow::resizeEvent(event);

  const int width = static_cast<int>(static_cast<qreal>(event->size().width()) * devicePixelRatio());
  const int height = static_cast<int>(static_cast<qreal>(event->size().height()) * devicePixelRatio());
  emit windowResizedEvent(width, height);
}
