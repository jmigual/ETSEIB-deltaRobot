/// @file joystick.h
/// @brief Contains the XJoystick's class declaration
#ifndef XJOYSTICK_H
#define XJOYSTICK_H
#define SFML_DYNAMIC

#if defined(XJOYSTICK_LIBRARY)
#  define XJOYSTICKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XJOYSTICKSHARED_EXPORT Q_DECL_IMPORT
#endif


// QT libraries
#include <QObject>
#include <QTimer>
#include <QVector>

// SFML Libraris
#include <SFML/Window.hpp>

// Standard libraries
#include <cmath>
#include <utility>

using namespace sf;

/// The XJoystick's class is used to control the SFML Joystick's class 
/// with <em>signals and slots</em>
class XJOYSTICKSHARED_EXPORT XJoystick : public QObject
{
    Q_OBJECT
    
    /// Typedef for the String and int pair
    typedef std::pair< QString, int > P;
    
    /// Typedef to reduce code writing
    typedef Joystick J;
    
    /// Struct to handle the axis
    struct Axis {
        /// Contains the SFML joystick axis number
        J::Axis axis;
        
        /// Contains the axis name
        QString name;
        
        /// Default constructor
        Axis() {}
        
        /// Initialization constructor
        Axis(J::Axis a, const QString &s) : axis(a), name(s) {}
        
        /// Copy constructor
        Axis(const Axis &a) : axis(a.axis), name(a.name) {}        
    };
    
public:
    
    /// Basic enumerations
    enum {
        Count = J::Count,
        AxisCount = J::AxisCount,
        ButtonCount = J::ButtonCount
    };
    
    /// Struct to handle the info 
    struct Info {
        /// Contains the Joystick's ID
        int ID;
        
        /// Contains all the data
        QString name;
        
        /// Default Constructor
        Info() : ID(-1) {}
    };
    
    /// Default constructor
    XJoystick(int ID = -1, float filter = 0.001);
    
    /// Copy constructor
    XJoystick(const XJoystick &j);
    
    /// Default destructor
    ~XJoystick();
    
    /// Amount of joysticks connected
    inline int amount() { return _joys.size(); }
    
    /// True if there's any joystick connected
    inline bool anyConnected() { return _joys.size() > 0; }
    
    /// Returns the current number of axes
    inline int axisCount() { return _aName.size(); }
    
    /// Returns the system available joysticks
    QVector< Info > available();
    
    /// Returns the button state
    bool button(int n);
    
    /// Returns the number of buttons
    inline int buttonCount() { return _butCount; }
    
    /// @brief Returns the current selected joystick, -1 if there's no selected 
    /// joystick
    inline int current() { return _ID; }
    
    /// Returns the all axes names
    QVector< QString > getAllAxis();
    
    /// Returns the current Joystick's axis and it's names
    inline QVector< P > getAxis() { return _aName; }
    
    /// To mantain an axis in a certain value
    void axisPress(unsigned char a, float value = 100);
    
    /// To release an axis and restore it's value with the joystick
    void axisRelease(unsigned char a);
    
    /// Returns the 'n' axis value
    inline const float& operator[] (int n) const { return _axis[n]; }
    
    /// Selects the especified joystick
    bool select(int s);
    
public slots:
    /// Updates all data
    void update();
    
signals:
    /// Emmitted when a joystick is connected or disconnected
    void changed();
    
private:
    
    /// Contains the joystick available axis
    QVector< P > _aName;
    
    /// Usefull to work with axis
    QVector< Axis > _aID;
    
    /// Contains the joystick axis values
    QVector< float > _axis;
    
    /// To hold the axis value
    QVector< bool > _axisH;
    
    /// Contains the number of buttons
    int _butCount;
    
    /// Contains the button values
    QVector< bool > _buttons;
    
    /// True if the filter must be activated
    float _filter;
    
    /// Selected Joystick ID
    int _ID;
    
    /// Contains the connected joysticks
    QVector< int > _joys;
    
    /// Timer to Handle elements
    QTimer _timer;
    
    /// Creates the default constructions
    void defaultConstruct();
    
    /// Filter to remove small values
    void filter();
    
private slots:
    /// Called to update the current connected joysticks
    void loopEvent();
    
};

#endif // XJOYSTICK_H
