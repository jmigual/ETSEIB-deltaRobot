using UnityEngine;
using System.Collections;

public class RobotCreator : MonoBehaviour {

	public float L1=3f,a=7f,b=10f,L2=2f;
	public Vector3 O;
	public Color Base, arms, forearms, platform;
	public float theta1=60f,theta2=60f,theta3=60f;

	private const float rad = 2f*Mathf.PI/360f;

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
		line=0;
		P=transform.position;
		setAngles ();
		drawBase ();
		drawArms ();
		drawPlatform();
		drawForearm();
	}
	void setAngles (){
		/* Su y=mi -z
		 * Su z=mi -y
		 * Su x=mi x
		 * P<-->O
		 * Su La=mi L1
		 * Su Lb=mi L2
		 * su L1=mi a
		 * su L2=mi b
		 * su theta=mi -theta
		 */
		float x0=P.x-O.x, y0=-O.z+P.z, z0=O.y-P.y;
		float A=1+(L1-y0+L2*L2)/z0;
		float B=2*((L1-y0+L2)/z0)*((b*b-x0*x0-z0*z0-a*a-L1*L1)/(2*z0))-2*L1;
		float C=((b*b-x0*x0-z0*z0-a*a-L1*L1*L1*L1)/(2*z0))-L1*L1-a*a;
		float Cy=(-B+Mathf.Sqrt(B*B-4*A*C))/(2*A);
		float Cz=Mathf.Sqrt (b*b-x0*x0-(Cy-y0+L2)*(Cy-y0+L2))+z0;
		theta1=-Mathf.Atan(Cz/(L1-Cy))/rad;
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
