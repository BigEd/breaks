#include "MyGraphicsView.h"

//Qt includes
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTextStream>
#include <QScrollBar>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>
#include <QMessageBox>

/**
* Sets up the subclassed QGraphicsView
*/
MyGraphicsView::MyGraphicsView(QWidget* parent, DebugContext * debug) : QGraphicsView(parent) {

    initialized = false;

    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    //Set-up the scene
    QGraphicsScene* Scene = new QGraphicsScene(parent);
    setScene(Scene);

    // load background image
    QImage image(debug->image);
    if (image.isNull()) QMessageBox::information(this, tr("Error"), tr("Cannot load background image: ") + debug->image );

    Scene->addPixmap(QPixmap::fromImage(image));

    //Set-up the view
    setSceneRect(0, 0, image.width(), image.height());

    //Use ScrollHand Drag Mode to enable Panning
    setDragMode(ScrollHandDrag);

    // Add step button.
    QPushButton *button = new QPushButton ( "Next Step", this);
    button->move(10, 10);
    button->show();
    connect (button, SIGNAL(clicked()), this, SLOT(NextStepPressed ()) );

    // Add triggers.
    GraphTrigger * trigs = debug->triggers;
    MyCheckBox * check;
    for (int n=0; n<debug->num_triggers; n++)
    {
        check = new MyCheckBox ( trigs[n].name, 0, trigs[n].link, this );
        Scene->addWidget (check);
        check->move( trigs[n].x, trigs[n].y );
        check->show();

        if ( * check->trigger_link == 0 ) check->setChecked( false );
        else check->setChecked( true );

        triggers[n] = check;
    }

    // Add locators.
    for (int i=0, dy=40; i<debug->num_locators; i++ ) {
        QPushButton *locator = new QPushButton ( debug->locators[i].name, this);
        locator->move(10, dy);
        locator->show();
        locator->setProperty( "index", i );
        connect (locator, SIGNAL(clicked()), this, SLOT(LocatorPressed()) ) ;
        dy += 20;
    }

    // Add collectors
    for (int i=0; i<debug->num_collectors; i++)
    {
        TextEdit * edit = new TextEdit (this);
        Scene->addWidget (edit);
        edit->setGeometry ( QRect(debug->collectors[i].coord_x, debug->collectors[i].coord_y, debug->collectors[i].width, debug->collectors[i].height) );
        edit->setFont( QFont(debug->collectors[i].font, debug->collectors[i].fontSize) );
        edit->setProperty ( "index", i );

        collectors[i] = edit;
    }

    debugContext = debug;
    initialized = true;

    setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Preferred);
}

/**
  * Zoom the view in and out.
  */
void MyGraphicsView::wheelEvent(QWheelEvent* event) {

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    // Scale the view / do the zoom
    double scaleFactor = 1.15;
    if(event->delta() > 0) {
        // Zoom in
        scale(scaleFactor, scaleFactor);
    } else {
        // Zooming out
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }

    // Don't call superclass handler here
    // as wheel is normally used for moving scrollbars
}

void MyGraphicsView::NextStepPressed ()
{
    debugContext->step ();

    // update triggers with new values
    updateTriggers();

    // update collectors with new values
    updateCollectors();
}

void MyGraphicsView::LocatorPressed ()
{
    QPushButton *button = (QPushButton *)sender ();

    int index = button->property("index").toInt();

    if ( index < debugContext->num_locators ) {
        resetTransform();
        QScrollBar * xPos = horizontalScrollBar ();
        xPos->setValue((int) debugContext->locators[index].coord_x);
        QScrollBar * yPos = verticalScrollBar();
        yPos->setValue((int) debugContext->locators[index].coord_y);
    }
}

void MyGraphicsView::updateTriggers ()
{
    if (initialized )
    {
        for (int i=0; i<debugContext->num_triggers; i++)
        {
            if ( * triggers[i]->trigger_link == 0 ) triggers[i]->setChecked( false );
            else triggers[i]->setChecked( true );
        }
    }
}

void MyGraphicsView::updateCollectors()
{
    if ( initialized )
    {
        for (int i=0; i<debugContext->num_collectors; i++)
        {
            if ( debugContext->collectors[i].getter ) {
                unsigned value = debugContext->collectors[i].getter ();
                QString string;
                collectors[i]->setText ( string.sprintf ( debugContext->collectors[i].format, value) );
            }
        }
    }
}
