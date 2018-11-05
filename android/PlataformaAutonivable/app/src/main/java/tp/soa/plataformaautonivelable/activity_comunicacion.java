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
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;


/*
*
*
*
*
* */

//******************************************** Hilo principal del Activity**************************************
public class activity_comunicacion extends Activity implements SensorEventListener
{

    Handler bluetoothIn;
    final int handlerState = 0; //used to identify handler message
    public SensorManager mSensorManager;
    private BluetoothAdapter btAdapter = null;
    private BluetoothSocket btSocket = null;
    private StringBuilder recDataString = new StringBuilder();
    private TextView textEstadoLed;
    private TextView textEstadoProximidad;
    private TextView textEstadoAltura;
    private Button  buttonDesconectar;
    private Boolean activarLed = false;
    private Boolean activarProximidad = false;
    private Boolean subirmesa = false;
    private double altura;

    private boolean sincronizarAcelerometro = false;
    private boolean sincronizarProximidad = false;
    private boolean sincronizarLuz = false;

    private ConnectedThread mConnectedThread;

    private static  final int cota_LED = 5;
    private static  final int cota_proximidad = 2;
    private static final char caracterSensorLuz = '@';
    private static final char caracterSensorProximidad = '&';
    private static final char caracterSensorAcelerometro = '#';


    private static final char caracterReciboLuz = '-';
    private static final char caracterReciboProximidad = '/';
    private static final char caracterReciboAcelerometro = '_';

    private static final UUID BTMODULEUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

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
        buttonDesconectar = (Button) findViewById(R.id.buttonDesconectar);
        bluetoothIn = Handler_Msg_Hilo_Principal();

        buttonDesconectar.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                try
                {
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

    }

    @Override
    //Cada vez que se detecta el evento OnResume se establece la comunicacion con el HC05, creando un
    //socketBluethoot
    public void onResume() {
        super.onResume();

        //Obtengo el parametro, aplicando un Bundle, que me indica la Mac Adress del HC05
        Intent intent=getIntent();
        Bundle extras=intent.getExtras();

        address= extras.getString("Direccion_Bluethoot");

        BluetoothDevice device = btAdapter.getRemoteDevice(address);
        Ini_Sensores();
        try
        {
            btSocket = createBluetoothSocket(device);
        }
        catch (IOException e)
        {
            showToast( "La creacción del Socket fallo");
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

        //Una establecida la conexion con el Hc05 se crea el hilo secundario, el cual va a recibir
        // los datos de Arduino atraves del bluethoot
        mConnectedThread = new ConnectedThread(btSocket);
        mConnectedThread.start();

        if(!sincronizarAcelerometro || !sincronizarLuz || !sincronizarProximidad) {

            mConnectedThread.write(caracterSensorAcelerometro + "ZZ" + caracterSensorAcelerometro);
            mConnectedThread.write(caracterSensorLuz + "ZZ" + caracterSensorLuz);
            mConnectedThread.write(caracterSensorProximidad + "ZZ" + caracterSensorProximidad);
        }
    }


    @Override
    //Cuando se ejecuta el evento onPause se cierra el socket Bluethoot, para no recibiendo datos
    public void onPause()
    {
        super.onPause();
        try
        {

            btSocket.close();
            sincronizarAcelerometro = false;
            sincronizarLuz = false;
            sincronizarProximidad = false;

        } catch (IOException e2) {
            //insert code to deal with this
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
                    int endOfLineIndex = recDataString.indexOf("\r\n");

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

    private void procesarMensaje(String mensaje)
    {
        char inicio = mensaje.charAt(0);
        String[] m = null;
        switch (inicio)
        {
            case caracterReciboAcelerometro:
                m = mensaje.split(Character.toString(caracterReciboAcelerometro));
                textEstadoAltura.setText(m[1] + " cms");
                sincronizarAcelerometro = true;
                break;
            case caracterReciboLuz:
                m = mensaje.split(Character.toString(caracterReciboLuz));
                textEstadoLed.setText(m[1]);
                sincronizarLuz = true;
                break;
            case caracterReciboProximidad:
                m = mensaje.split(Character.toString(caracterReciboProximidad));
                textEstadoProximidad.setText(m[1]);
                sincronizarProximidad = true;
                break;
            default:
                break;
        }


    }

    private void showToast(String message) {
        Toast.makeText(getApplicationContext(), message, Toast.LENGTH_SHORT).show();
    }

    protected void Ini_Sensores()
    {
        mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER),   SensorManager.SENSOR_DELAY_NORMAL);
        mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_LIGHT), SensorManager.SENSOR_DELAY_NORMAL);
        mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_PROXIMITY),       SensorManager.SENSOR_DELAY_NORMAL);
        mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD),SensorManager.SENSOR_DELAY_NORMAL);
    }
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

        // Cada sensor puede lanzar un thread que pase por aqui
        // Para asegurarnos ante los accesos simultaneos sincronizamos esto

        if(sincronizarProximidad && sincronizarLuz && sincronizarAcelerometro) {
            synchronized (this) {
                Log.d("sensor", event.sensor.getName());

                switch (event.sensor.getType()) {

                    case Sensor.TYPE_ACCELEROMETER:
                        if (esInclinacionValida((double) event.values[0], (double) event.values[1], (double) event.values[2])) {
                            altura = obtenerAlturaMesa(event.values[1]);
                            textEstadoAltura.setText(altura + " cms");
                            mConnectedThread.write("#" + altura + "#");
                        }

                        break;

                    case Sensor.TYPE_PROXIMITY:

                        if (event.values[0] < cota_proximidad && !activarProximidad) {

                            activarProximidad = true;
                            if (subirmesa) {

                                textEstadoProximidad.setText("Proximidad detectada : Bajando plataforma");
                                subirmesa = false;
                                mConnectedThread.write("&1&");
                            } else {

                                textEstadoProximidad.setText("Proximidad detectada : Subiendo plataforma");
                                subirmesa = true;
                                mConnectedThread.write("&2&");
                            }
                        } else if (event.values[0] >= cota_proximidad) {

                            activarProximidad = false;
                        }

                        break;

                    case Sensor.TYPE_LIGHT:

                        if (event.values[0] < cota_LED) {

                            textEstadoLed.setText("Baja luminosidad : Prender LED");
                            mConnectedThread.write("*1*");
                        } else if (event.values[0] >= cota_LED) {

                            textEstadoLed.setText("Luminosidad detectada : Apagar LED");
                            mConnectedThread.write("*2*");
                        }
                        break;

                }
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


    // Metodo que escucha el cambio de sensibilidad de los sensores
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

    @Override
    protected void onStop()
    {

        try
        {

            btSocket.close();
            sincronizarAcelerometro = false;
            sincronizarLuz = false;
            sincronizarProximidad = false;
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
