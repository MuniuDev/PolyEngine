#include "PolyDockWidgetManager.hpp"
#include "PolyEditorPCH.hpp"

PolyDockManager::PolyDockManager()
{
}

PolyDockManager::~PolyDockManager()
{
}

void PolyDockManager::WidgetCatchEvent(PolyWidget* catched)
{
	DraggedWidget = catched;
}

void PolyDockManager::ProcessEvent(QEvent* event)
{
	if (DraggedWidget != nullptr)
		if (event->type() == QEvent::MouseMove)
		{
			WidgetMoveEvent(event);

			if (MouseOver && MouseOver != DraggedWidget->DockWidget->parent())
			{
				QPoint pos = ((QMainWindow*)DraggedWidget->DockWidget->parent())->pos();
				QSize size = ((QMainWindow*)DraggedWidget->DockWidget->parent())->size();
				((QMainWindow*)DraggedWidget->DockWidget->parent())->move(MouseOver->pos());
				((QMainWindow*)DraggedWidget->DockWidget->parent())->resize(MouseOver->size());
				MouseOver->move(pos);
				MouseOver->resize(size);

				//for (int i = 0; i < ((PolyWindow*)DraggedWidget->DockWidget->parent())->Widgets.GetSize(); i++)
				//{
				//	if (((PolyWindow*)DraggedWidget->DockWidget->parent())->Widgets[i] == DraggedWidget) continue;
				//
				//	MouseOver->AddWidget(Qt::DockWidgetArea::BottomDockWidgetArea, ((PolyWindow*)DraggedWidget->DockWidget->parent())->Widgets[i]);
				//}
			}
		}
		else if (event->type() == QEvent::MouseButtonRelease)
			WidgetDropEvent(event);
}

void PolyDockManager::WidgetMoveEvent(QEvent * event)
{
	QPoint mousePos = ((QMouseEvent*)event)->pos() + DraggedWidget->GetDockWidget()->pos();
	MouseOver = nullptr;

	for (int i = 0; i < gApp->Ui.Windows.GetSize(); i++)
	{
		QPoint diff = mousePos - gApp->Ui.Windows[i]->pos();

		if (diff.x() >= 0 && diff.x() < gApp->Ui.Windows[i]->size().width() &&
			diff.y() >= 0 && diff.y() < gApp->Ui.Windows[i]->size().height())
		{
			MouseOver = gApp->Ui.Windows[i];
			break;
		}
	}
}

void PolyDockManager::WidgetDropEvent(QEvent* event)
{
	if (!MouseOver)
	{
		PolyWindow* wnd = new PolyWindow;
		wnd->resize(DraggedWidget->GetDockWidget()->size());
		wnd->move(DraggedWidget->GetDockWidget()->pos());
		wnd->AddWidget(Qt::DockWidgetArea::TopDockWidgetArea, DraggedWidget);
		wnd->show();
		gApp->Ui.Windows.PushBack(std::move(wnd));
	}

	DraggedWidget = nullptr;
}
