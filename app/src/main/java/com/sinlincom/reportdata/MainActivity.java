package com.sinlincom.reportdata;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.Intent;
import android.location.Criteria;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.provider.Settings;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;
import android.telephony.TelephonyManager;
import android.widget.Toast;

import java.io.IOException;
import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Timer;
import com.sinlincom.reportdata.SwitchData;
import com.sinlincom.reportdata.ImitateData;
import  com.sinlincom.http.*;

import org.json.JSONObject;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
    int funType;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        String showStr="showStr:";
      //  Object jobject = getReportData(funType);

       /* try{
            Runtime.getRuntime().exec("su");
            System.out.print("run su");

            Log.d("ioexception","run not exceptipon");
        }catch (IOException e)
        {
            System.out.print("ioexception");
            Log.d("ioexception","ioexcepiton"+e.getMessage());
        }*/

       try{
           Runtime.getRuntime().exec("su");
           Log.d("RunTimeexception","RunTimeexception"+"su success");
       }catch(IOException e)
       {
           Log.d("RunTimeexception","RunTimeexception"+e.getMessage());
       }

        getReportData(funType);

      /*  Context context  =  getApplicationContext();
        int slotId = 0;
        String strImel = getIMEI(context,slotId);  //imel 号




        if (funType == 1)//开关量部分
        {
            SwitchData sdata = (SwitchData)jobject;
           showStr = showStr +  "sData "+  "id :" + String.valueOf(sdata.id) + "len :" + String.valueOf(sdata.len) + "data:" +  String.valueOf(sdata.data) + "crc:" + sdata.crc +"fun:" + String.valueOf(sdata.fun) ;
        } else if (funType == 3) // 模拟量部分
        {
            ImitateData idata = (ImitateData) jobject;
            showStr = showStr +  "IData "+  "dtuid :" + String.valueOf(idata.dtuid) + "fun :" + String.valueOf(idata.fun) + "len:" +  String.valueOf(idata.len) + "one:" + idata.oneImitateData +"two" + idata.twoImitateData + "three:" + idata.threeImitateData +"strikecount:" + idata.strikeCount + "strikePeek:" + idata.strikePeak ;
        } else {
            showStr = showStr + "showEmpty";
        }
    */

       /* AsyncHttpClient clientG = new AsyncHttpClient();
        clientG.get("https://www.baidu.com", new TextResponseHandler() {
            @Override
            public void onSuccess(String response) {

            }

            @Override
            public void onFailure(Throwable e) {

            }
        });

        AsyncHttpClient clientP = new AsyncHttpClient();
        HashMap<String, String> map = new HashMap<>();
        map.put("key", "value");
        clientP.post("http://xxx.xxx.com/xxx", map, new JsonObjectResponseHandler() {

            @Override
            public void onFailure(Throwable e) {
                // TODO
            }

            @Override
            public void onSuccess(JSONObject response) {
                // TODO
            }
        });


        TextView tv = findViewById(R.id.sample_text);
        tv.setText(showStr);
        */
    }



    public void setTextView()
    {
        String text = "c  call";
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(text);
        Log.d("mainActivity","settextview");
    }

    //判断是否打开GPS
    private void openGPSSettings(){
        LocationManager alm = (LocationManager)this.getSystemService(Context.LOCATION_SERVICE);
        if (alm.isProviderEnabled(android.location.LocationManager.GPS_PROVIDER)) {
            Toast.makeText(this, "GPS模块正常" ,Toast.LENGTH_SHORT).show();
            return;
        }
        Toast.makeText(this, "请开启GPS！" ,Toast.LENGTH_SHORT).show();
        Intent intent = new Intent(Settings.ACTION_SECURITY_SETTINGS);
        startActivityForResult(intent,0); //此为设置完成后返回到获取界面
    }


    @SuppressLint("MissingPermission")
    private void getLocation(){
        // 获取位置管理服务
        LocationManager locationManager;
        String serviceName = Context.LOCATION_SERVICE;
        locationManager = (LocationManager)this.getSystemService(serviceName);
        // 查找到服务信息
        Criteria criteria = new Criteria();
        criteria.setAccuracy(Criteria.ACCURACY_FINE);
        // 高精度
        criteria.setAltitudeRequired(false);
        criteria.setBearingRequired(false);
        criteria.setCostAllowed(true);
        criteria.setPowerRequirement(Criteria.POWER_LOW); // 低功耗
        String provider =locationManager.getBestProvider(criteria, true); // 获取GPS信息
        @SuppressLint("MissingPermission") Location location =locationManager.getLastKnownLocation(provider); // 通过GPS获取位置
        updateToNewLocation(location);             // 设置监听*器，自动更新的最小时间为间隔N秒(1秒为1*1000，这样写主要为了方便)或最小位移变化超过N米
        //locationManager.requestLocationUpdates(provider,100 * 1000, 500,locationListener);
        locationManager.requestLocationUpdates(provider, 100 * 1000, 500, new LocationListener() {
            @Override
            public void onLocationChanged(Location location) {

            }

            @Override
            public void onStatusChanged(String provider, int status, Bundle extras) {

            }

            @Override
            public void onProviderEnabled(String provider) {

            }

            @Override
            public void onProviderDisabled(String provider) {

            }
        });
    }


    private void updateToNewLocation(Location location){
        TextView tv1;
        tv1 = (TextView)this.findViewById(R.id.sample_text);
        if (location != null) {
            double latitude = location.getLatitude();
            double longitude=location.getLongitude();
            tv1.setText( "维度：" + latitude+ "\n经度 "+longitude);
        } else {
            tv1.setText( "无法获取地理信息" );
        }
    }


    public static String getIMEI(Context context, int slotId) {
        try {
            TelephonyManager manager = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
            Method method = manager.getClass().getMethod("getImei", int.class);
            String imei = (String) method.invoke(manager, slotId);
            return imei;
        } catch (Exception e) {
            return "";
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
    public native void callMethod1();
    public  native Object getReportData( int type);
    public native  SwitchData testTransStruct();
}
