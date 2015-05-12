using UnityEngine;
using System.Collections;

public class RobotCreator : MonoBehaviour {

	public float L1=3f,a=7f,b=10f,L2=2f;
	public Vector3 O;
	public Color Base, arms, forearms, platform;
	public float theta1=60f,theta2=60f,theta3=60f;
	public bool controlled=false;
	public bool calculateWorkspace=true;

	public Vector3 Ocontrolled;
	public bool valid=true;

	private const float rad = 2f*Mathf.PI/360f;
	//private int signe = 1;
	private LineRenderer[] Lines;
	private int line;
	//plataforma base
	private Vector3 P,D1,D2,D3;
	//braç
	private Vector3 J1,J2,J3;


	//plataforma pinça
	private Vector3 E1,E2,E3;

	void Start () {
		line=0;
		Lines = GetComponentsInChildren <LineRenderer> ();
		for (int i=0; i<Lines.Length; ++i)Lines[i].enabled = false;
	}

	void addLine (Vector3 a, Vector3 b, Color col){
		Lines[line].SetVertexCount(2);
		Lines[line].enabled=true;
		Lines[line].SetColors(col,col);
		Lines[line].SetPosition(0,a);
		Lines[line].SetPosition(1,b);
		line++;
	}
	void addTriangle (Vector3 a, Vector3 b, Vector3 c, Color col){
		Lines[line].SetVertexCount(4);
		Lines[line].enabled=true;
		Lines[line].SetColors(col,col);
		Lines[line].SetPosition(0,a);
		Lines[line].SetPosition(1,b);
		Lines[line].SetPosition(2,c);
		Lines[line].SetPosition(3,a);
		line++;
	}
	void Update () {
		if (controlled)O=Ocontrolled;
		line=0;
		P=transform.position;
		drawBase ();
		drawPlatform();
		setAngles ();
		drawArms ();
		drawForearm();
	}
	public void setAngles (){
		if (calculateWorkspace)O=Ocontrolled;
		float cos60=Mathf.Cos (60*rad);
		float sin60=Mathf.Sin (60*rad);

		float x0,y0,z0;
		x0=O.z-P.z;
		y0=O.y-P.y;
		z0=O.x-P.x;

		float x1,y1,z1;
		x1=x0+L2-L1;
		y1=y0;
		z1=z0;
		theta1=singleAngle (x1,y1,z1,a,b);

		float x2,y2,z2;
		x2=z0*sin60-x0*cos60+L2-L1;
		y2=y0;
		z2=-z0*cos60-x0*sin60;
		theta2=singleAngle (x2,y2,z2,a,b);

		float x3,y3,z3;
		x3=-z0*sin60-x0*cos60+L2-L1;
		y3=y0;
		z3=-z0*cos60+x0*sin60;
		theta3=singleAngle (x3,y3,z3,a,b);

		
		valid=(!float.IsNaN(theta1) && !float.IsNaN(theta2) && !float.IsNaN(theta3));
	}
	float singleAngle (float x0, float y0, float z0, float r1, float r2){
		float n = r2 * r2 - r1 * r1 - z0 * z0 - x0 * x0 - y0 * y0;
		float raiz = Mathf.Sqrt (n * n * y0 * y0 - 4 * (x0 * x0 + y0 * y0) * (-x0 * x0 * r1 * r1 + n * n / 4));
		if (x0 < 0)raiz = -raiz;
		float y = (-n*y0 + raiz ) / (2*(x0*x0+y0*y0));
		int signe=1;
		if ((r2*r2-(y0+r1)*(y0+r1))<(x0*x0+z0*z0) && x0<0)signe = signe * -1;
		float x = Mathf.Sqrt(r1 * r1 - y * y)*signe;
		return -Mathf.Atan2 (y,x)*180/Mathf.PI;
	}
	void drawBase(){
		D1.Set(0,0,L1);
		D1+=P;
		D2.Set(L1*Mathf.Sin(60f*rad),0,-L1*Mathf.Cos(60f*rad));
		D2+=P;  
		D3.Set(-L1*Mathf.Sin(60f*rad),0,-L1*Mathf.Cos(60f*rad));
		D3+=P;
		addTriangle (D1,D2,D3, Base);
	}
	void drawArms(){
		J1.Set (0,-a*Mathf.Sin (theta1*rad),a*Mathf.Cos(theta1*rad));
		J1+=D1;
		addLine(D1,J1, arms);
		J2.Set (a*Mathf.Cos(theta2*rad)*Mathf.Sin(60f*rad),-a*Mathf.Sin (theta2*rad),-a*Mathf.Cos(60f*rad)*Mathf.Cos(theta2*rad));
		J2+=D2;
		addLine(D2,J2, arms);
		J3.Set (-a*Mathf.Cos(theta3*rad)*Mathf.Sin(60f*rad),-a*Mathf.Sin (theta3*rad),-a*Mathf.Cos(60f*rad)*Mathf.Cos(theta3*rad));
		J3+=D3;
		addLine(D3,J3, arms);
	}
	void drawPlatform(){
		E1.Set(0,0,L2);
		E1+=O;
		E2.Set(L2*Mathf.Sin(60f*rad),0,-L2*Mathf.Cos(60f*rad));
		E2+=O;  
		E3.Set(-L2*Mathf.Sin(60f*rad),0,-L2*Mathf.Cos(60f*rad));
		E3+=O;
		addTriangle (E1,E2,E3, platform);
	}
	void drawForearm(){
		addLine (J1,E1, forearms);
		addLine (J2,E2, forearms);
		addLine (J3,E3, forearms);
	}
}
