package tp.soa.plataformaautonivelable;


import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.LinkedList;
import java.util.Queue;
import java.util.UUID;

import static java.lang.Math.abs;


/*
*
*
*
*
* */

//******************************************** Hilo principal del Activity**************************************
public class activity_comunicacion extends AppCompatActivity implements SensorEventListener
{

    Handler bluetoothIn;
    final int handlerState = 0; //used to identify handler message
    public SensorManager mSensorManager;
    private BluetoothAdapter btAdapter = null;
    private BluetoothSocket btSocket = null;
    private StringBuilder recDataString = new StringBuilder();
    private TextView textModo;
    private TextView textEstadoLed;
    private TextView textEstadoProximidad;
    private TextView textEstadoAltura;
    private Button  buttonDesconectar;
    private Button  buttonCambiarModo;
    private Button buttonSetInclinacion;

    boolean enviarInclinacion = false;

    //Banderas usadas para evitar que cambios en los sensores de luz/proximidad por debajo de la cota vuelvan a
    // comunicarse con el embebido
    private Boolean activarLed = false;
    private Boolean activarProximidad = false;
    private int modoActual = 1;

    //Bandera para indicar si la mesa debe subir o bajar
    private Boolean subirmesa = false;


    private double altura;

    //Bandera para detectar si puedo enviar un mensaje al embebido
    private boolean banderaEnvio = true;

    //Hilo que realiza la comunicacion con el embebido
    private ConnectedThread mConnectedThread;
    //Hilo que recorre y procesa la cola de mensajes
    //private HiloEnvio mHiloEnvio;

    //Limite al cual se detecta cambio en el sensor de luz
    // Si es menor: No hay luz
    // Si es mayor: Hay luz
    private static  final int cota_LED = 50;

    //Limite al cual se detecta cambio en el sensor de proximidad
    // Si es menor: Detecta proximidad
    // Si es mayor: Deja de detectar proximidad
    private static  final int cota_proximidad = 2;


    //Caracteres usados para que el embebido detecte a que sensor corresponde el mensaje enviado
    private static final char caracterSensorLuz = '@';
    private static final char caracterSensorProximidad = '&';
    private static final char caracterSensorAcelerometro = '#';

    //Caracteres usados para que android detecte a que sensor corresponde el mensaje recibido
    private static final char caracterReciboLuz = '-';
    private static final char caracterReciboProximidad = '/';
    private static final char caracterReciboAcelerometro = '_';

    //Cola de mensajes
    private Queue<String> cola;

    private static final UUID BTMODULEUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    //MAC address del Bluetooth
    private static String address = null;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_comunicacion);

        btAdapter = BluetoothAdapter.getDefaultAdapter();
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        textEstadoAltura = (TextView) findViewById(R.id.textViewAltura);
        textEstadoProximidad = (TextView) findViewById(R.id.textViewProximidad);
        textEstadoLed = (TextView) findViewById(R.id.textViewEstadoLed);
        textModo = (TextView) findViewById(R.id.textViewModo);
        buttonDesconectar = (Button) findViewById(R.id.buttonDesconectar);
        buttonCambiarModo = (Button) findViewById(R.id.buttonCambiarModo);
        buttonSetInclinacion = (Button) findViewById(R.id.buttonSetInclinacion);
        bluetoothIn = Handler_Msg_Hilo_Principal();
        cola = new LinkedList<String>();
        buttonDesconectar.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                try
                {
                    mConnectedThread.write("MODE 1;");
                    btSocket.close();
                    btAdapter.disable();
                    Intent i = new Intent(getApplicationContext(),MainActivity.class);
                    startActivity(i);
                }
                catch (IOException e2)
                {
                    //insert code to deal with this
                }

            }
        });

        buttonCambiarModo.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                    if (modoActual == 1) {
                        modoActual = 2;
                    } else {
                        modoActual = 1;
                    }
                    mConnectedThread.write("MODE " + modoActual + ";");
                    System.out.println("----- ENVIE MODO " + modoActual + " -----");
            }
        });

        buttonSetInclinacion.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                enviarInclinacion = true;
            }
        });
    }

    @Override
    //En el metodo onResume se inicializan los threads, se crea el bluetooth socket y se envian los
    //mensajes de sincronizacion
    public void onResume() {
        super.onResume();

        //Obtengo el parametro, aplicando un Bundle, que me indica la Mac Adress del HC05
        Intent intent=getIntent();
        Bundle extras=intent.getExtras();

        address= extras.getString("Direccion_Bluethoot");

        BluetoothDevice device = btAdapter.getRemoteDevice(address);

        try
        {
            btSocket = createBluetoothSocket(device);
        }
        catch (IOException e)
        {
            showToast( "La creación del Socket fallo");
        }

        try
        {
            btSocket.connect();
        }
        catch (IOException e)
        {
            try
            {
                btSocket.close();
            }
            catch (IOException e2)
            {
                //insert code to deal with this
            }
        }

        mConnectedThread = new ConnectedThread(btSocket);
        mConnectedThread.start();
       mConnectedThread.write("STAT;");
    }


    @Override
    //Cuando se ejecuta el evento onPause se cierra el socket Bluethoot, para no recibiendo datos
    public void onPause()
    {
        super.onPause();
        try
        {

            btSocket.close();
        } catch (IOException e2) {

        }
    }

    //Metodo que crea el socket bluethoot
    private BluetoothSocket createBluetoothSocket(BluetoothDevice device) throws IOException {

        return  device.createRfcommSocketToServiceRecord(BTMODULEUUID);
    }

    //Handler que sirve que permite mostrar datos en el Layout al hilo secundario
    private Handler Handler_Msg_Hilo_Principal ()
    {
        return new Handler() {
            public void handleMessage(android.os.Message msg)
            {
                //si se recibio un msj del hilo secundario
                if (msg.what == handlerState)
                {
                    //voy concatenando el msj
                    String readMessage = (String) msg.obj;
                    recDataString.append(readMessage);
                    int endOfLineIndex = recDataString.indexOf(";");

                    //cuando recibo toda una linea la muestro en el layout
                    if (endOfLineIndex > 0)
                    {
                        String dataInPrint = recDataString.substring(0, endOfLineIndex);
                        Toast.makeText(activity_comunicacion.this, recDataString, Toast.LENGTH_SHORT).show();
                        procesarMensaje(recDataString.toString());
                        recDataString.delete(0, recDataString.length());
                    }
                }
            }
        };

    }

    //Funcion para procesar los mensajes que recibo del bluetooth
    private void procesarMensaje(String mensaje)
    {
        String[] m = null;
        m = mensaje.split(" ");
        String inicio = m[0];
        switch (inicio)
        {
            case "RSTA":
                refrescarTextoModo();
                textEstadoLed.setText("Estado LED: " + m[2]);
                textEstadoAltura.setText("Altura actual: " + m[3]);
                break;
            case "-RMOD":
                if(m[1].compareTo("OK;") == 0)
                {
                    Toast.makeText(activity_comunicacion.this, "Modo modificado correctamente", Toast.LENGTH_LONG).show();
                    refrescarTextoModo();
                    Ini_Sensores();
                }
                else
                    Toast.makeText(activity_comunicacion.this, "Error al modificar modo", Toast.LENGTH_LONG).show();
                break;
            case "-MAXH":
                    Toast.makeText(activity_comunicacion.this, "Altura maxima nivelada", Toast.LENGTH_LONG).show();
                break;
            case "-MINH":
                    Toast.makeText(activity_comunicacion.this, "Altura minima nivelada", Toast.LENGTH_LONG).show();
                break;
            case "-NERR":
                    Toast.makeText(activity_comunicacion.this, "No es posible nivelar", Toast.LENGTH_LONG).show();
                break;
            default:
                banderaEnvio = true;
                break;
        }


    }

    private void refrescarTextoModo() {
        textModo.setText(Integer.toString(modoActual));
    }

    private void showToast(String message) {
        Toast.makeText(getApplicationContext(), message, Toast.LENGTH_SHORT).show();
    }

    //Inicializar los sensores
    protected void Ini_Sensores()
    {
        mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER),   SensorManager.SENSOR_DELAY_NORMAL);
        mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_LIGHT), SensorManager.SENSOR_DELAY_NORMAL);
        mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_PROXIMITY),       SensorManager.SENSOR_DELAY_NORMAL);
    }

    //Parar la deteccion de los sensores
    private void Parar_Sensores()
    {

        mSensorManager.unregisterListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER));
        mSensorManager.unregisterListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_PROXIMITY));
        mSensorManager.unregisterListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_LIGHT));
    }


    // Metodo que escucha el cambio de los sensores
    @Override
    public void onSensorChanged(SensorEvent event)
    {
        String txt = "";

            synchronized (this) {
                Log.d("sensor", event.sensor.getName());

                switch (event.sensor.getType()) {

                    case Sensor.TYPE_ACCELEROMETER:
                        /*if (esInclinacionValida((double) event.values[0], (double) event.values[1], (double) event.values[2])) {
                            altura = obtenerAlturaMesa(event.values[1]);
                            textEstadoAltura.setText("Seteando altura a " + altura);
                            mConnectedThread.write("SETH " + altura + ";");
                        }*/

                        double inclinacionEjeY = (double) event.values[1];
                        System.out.println("inclinacion = " + inclinacionEjeY);
                        if(enviarInclinacion && abs(inclinacionEjeY) > 9.8/2) {
                            altura = inclinacionEjeY > 0 ? 1 : -1;
                            mConnectedThread.write("SETH " + altura + ";");
                        }
                        enviarInclinacion = false;

                        break;

                    case Sensor.TYPE_PROXIMITY:

                        if (event.values[0] < cota_proximidad && !activarProximidad) {

                            activarProximidad = true;
                            if (subirmesa) {

                                textEstadoProximidad.setText("Proximidad detectada : Bajando plataforma");
                                subirmesa = false;
                                mConnectedThread.write("SETP " + 1 + ";");
                                System.out.println(" ------- ENVIE ALTURA MINIMA ------- ");
                                //cola.add("&1&");
                            } else {

                                textEstadoProximidad.setText("Proximidad detectada : Subiendo plataforma");
                                subirmesa = true;
                                mConnectedThread.write("SETP " + 2 + ";");
                                System.out.println(" ------- ENVIE ALTURA MAXIMA ------- ");
                                //cola.add("&2s&");
                            }
                        } else if (event.values[0] >= cota_proximidad) {

                            activarProximidad = false;
                        }

                        break;

                    case Sensor.TYPE_LIGHT:

                        if (event.values[0] < cota_LED) {

                            textEstadoLed.setText("Baja luminosidad : Prender LED");
                            mConnectedThread.write("SETL " + 1 + ";");
                            //cola.add("*1*");
                        } else if (event.values[0] >= cota_LED) {

                            textEstadoLed.setText("Luminosidad detectada : Apagar LED");
                            mConnectedThread.write("SETL " + 0 + ";");
                            //cola.add("*2*");
                        }
                        break;

                }
            }

    }





    private boolean esInclinacionValida(double ejeX, double ejeY, double ejeZ){
        if(ejeX<minX || ejeX>maxX)
            return false;

        if(ejeY<minY || ejeY>maxY)
            return false;

        if(ejeZ<minZ || ejeZ>maxZ)
            return false;

        return true;
    }

    private  double minX = -2;
    private  double maxX = 2;
    private  double minY = 0;
    private  double maxY = 9.5;
    private  double minZ = 0;
    private  double maxZ = 9.5;


    private double obtenerAlturaMesa(double ejeY){
        double alturaMaxima = 40; //altura maxima de la mesa 50 cms (ejeY = maxY)
        double alturaMinima = 10; //altura minima de la mesa 10 cms (ejeY = minY)

        if (ejeY == 0)
            return 10;

        return Math.round((ejeY * alturaMaxima / maxY)+alturaMinima);
    }


    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy)
    {

    }

    //******************************************** Hilo secundario del Activity**************************************
    //*************************************** recibe los datos enviados por el HC05**********************************

    private class ConnectedThread extends Thread
    {
        private final InputStream mmInStream;
        private final OutputStream mmOutStream;

        //Constructor de la clase del hilo secundario
        public ConnectedThread(BluetoothSocket socket)
        {
            InputStream tmpIn = null;
            OutputStream tmpOut = null;

            try
            {
                //Create I/O streams for connection
                tmpIn = socket.getInputStream();
                tmpOut = socket.getOutputStream();
            } catch (IOException e) { }

            mmInStream = tmpIn;
            mmOutStream = tmpOut;
        }

        //metodo run del hilo, que va a entrar en una espera activa para recibir los msjs del HC05
        public void run()
        {
            byte[] buffer = new byte[256];
            int bytes;

            //el hilo secundario se queda esperando mensajes del HC05
            while (true)
            {
                try
                {
                    //se leen los datos del Bluethoot
                    bytes = mmInStream.read(buffer);
                    String readMessage = new String(buffer, 0, bytes);

                    //se muestran en el layout de la activity, utilizando el handler del hilo
                    // principal antes mencionado
                    bluetoothIn.obtainMessage(handlerState, bytes, -1, readMessage).sendToTarget();
                } catch (IOException e) {
                    break;
                }
            }
        }


        //write method
        public void write(String input) {
            byte[] msgBuffer = input.getBytes();           //converts entered String into bytes
            try {
                mmOutStream.write(msgBuffer);                //write bytes over BT connection via outstream
            } catch (IOException e) {
                //if you cannot write, close the application
                showToast("La conexion fallo");
                finish();

            }
        }
    }

    //Thread que recorre la cola de mensajes y envia
    /*private class HiloEnvio extends Thread
    {

        public HiloEnvio()
        { }

        //metodo run del hilo, que va a entrar en una espera activa para enviar mensajes que esten en la cola
        public void run()
        {
            while (true)
            {
                try
                {
                    if(!cola.isEmpty() && banderaEnvio)
                    {
                        banderaEnvio = false;
                        mConnectedThread.write(cola.poll());
                    }

                } catch (Exception e) {
                    break;
                }
            }
        }
    }*/

    @Override
    protected void onStop()
    {

        try
        {


            btSocket.close();
            Parar_Sensores();

        } catch (IOException e2) {
            //insert code to deal with this
        }

        super.onStop();
    }

    @Override
    protected void onDestroy()
    {
        Parar_Sensores();
        super.onDestroy();
    }
}
