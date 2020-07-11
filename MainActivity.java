package com.example.thankyouanonymous;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.telephony.SmsManager;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import java.util.Set;

public class MainActivity extends AppCompatActivity {
    private final int MY_PERMISSION_REQUEST_SMS = 1;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        //권한이 부여되어 있는지 확인
        int permissonCheck = ContextCompat.checkSelfPermission(this, Manifest.permission.SEND_SMS);

        if (permissonCheck == PackageManager.PERMISSION_GRANTED) {
            Toast.makeText(getApplicationContext(), "SMS 수신권한 있음", Toast.LENGTH_SHORT).show();
        } else {
            Toast.makeText(getApplicationContext(), "SMS 수신권한 없음", Toast.LENGTH_SHORT).show();

            //권한설정 dialog에서 거부를 누르면
            //ActivityCompat.shouldShowRequestPermissionRationale 메소드의 반환값이 true가 된다.
            //단, 사용자가 "Don't ask again"을 체크한 경우
            //거부하더라도 false를 반환하여, 직접 사용자가 권한을 부여하지 않는 이상, 권한을 요청할 수 없게 된다.
            if (ActivityCompat.shouldShowRequestPermissionRationale(this, Manifest.permission.SEND_SMS)) {
                //이곳에 권한이 왜 필요한지 설명하는 Toast나 dialog를 띄워준 후, 다시 권한을 요청한다.
                Toast.makeText(getApplicationContext(), "SMS권한이 필요합니다", Toast.LENGTH_SHORT).show();
                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.SEND_SMS}, MY_PERMISSION_REQUEST_SMS);
            } else {
                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.SEND_SMS}, MY_PERMISSION_REQUEST_SMS);
            }
        }
        int state1 = 0, state2 = 0;
        String msg = "연결이 끊어졌습니다.";
        final String phoneNum = "01079320496";
        BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter(); // 블루투스 어댑터를 디폴트 어댑터로 설정
        //SmsManager smsManager = SmsManager.getDefault();
        while (true) {


            if (bluetoothAdapter.isEnabled() && state1 == 0) {
                state1 = 1;
            }
            else if (!bluetoothAdapter.isEnabled() && state1 == 1) {

                try {
                    //전송
                    SmsManager smsManager = SmsManager.getDefault();
                    smsManager.sendTextMessage(phoneNum, null, msg, null, null);
                    Toast.makeText(getApplicationContext(), "전송 완료!", Toast.LENGTH_LONG).show();
                } catch (Exception e) {
                    Toast.makeText(getApplicationContext(), "SMS faild, please try again later!", Toast.LENGTH_LONG).show();
                    e.printStackTrace();
                }
                break;
            }


        }

        IntentFilter filter = new IntentFilter();
        filter.addAction(BluetoothDevice.ACTION_ACL_CONNECTED);
        filter.addAction(BluetoothDevice.ACTION_ACL_DISCONNECT_REQUESTED);
        filter.addAction(BluetoothDevice.ACTION_ACL_DISCONNECTED);
        this.registerReceiver(mReceiver, filter);
    }
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        int state = 0;
        String conMsg = "연결되었습니다.";
        String disMsg = "연결이 끊어졌습니다.";
        final String phoneNum = "01079320496";

        @Override
        public void onReceive(Context context, Intent intent) { // 연결이 되거나 끊길때 호출됨!!
            BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
            if(!bluetoothAdapter.isEnabled()){
                SmsManager smsManager = SmsManager.getDefault();
                smsManager.sendTextMessage(phoneNum, null, disMsg, null, null);
                Toast.makeText(getApplicationContext(), "전송 완료!", Toast.LENGTH_LONG).show();
            }else {
                if (state % 2 == 0) {
                    SmsManager smsManager = SmsManager.getDefault();
                    smsManager.sendTextMessage(phoneNum, null, conMsg, null, null);
                    Toast.makeText(getApplicationContext(), "전송 완료!", Toast.LENGTH_LONG).show();
                }
                if (state % 2 == 1) {
                    SmsManager smsManager = SmsManager.getDefault();
                    smsManager.sendTextMessage(phoneNum, null, disMsg, null, null);
                    Toast.makeText(getApplicationContext(), "전송 완료!", Toast.LENGTH_LONG).show();
                }
                state++;
            }
        }
    };
    
    /*
        while (true) {


            if (bluetoothAdapter.isEnabled() && state1 == 0) {
                state1 = 1;
            }
            else if (!bluetoothAdapter.isEnabled() && state1 == 1) {

                try {
                    //전송
                    SmsManager smsManager = SmsManager.getDefault();
                    smsManager.sendTextMessage(phoneNum, null, msg, null, null);
                    Toast.makeText(getApplicationContext(), "전송 완료!", Toast.LENGTH_LONG).show();
                } catch (Exception e) {
                    Toast.makeText(getApplicationContext(), "SMS faild, please try again later!", Toast.LENGTH_LONG).show();
                    e.printStackTrace();
                }
                break;
            }


        }*/


    public void onRequestPermissionResult(int requestCode, @NonNull String[] permission, @NonNull int[] grantResults){
        switch (requestCode){
            case MY_PERMISSION_REQUEST_SMS:{
                if(grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED){
                    Toast.makeText(this, "SMS권한 승인함", Toast.LENGTH_SHORT).show();
                }else{
                    Toast.makeText(this, "SMS권한 거부함", Toast.LENGTH_SHORT).show();
                }
            }
        }
    }
}
/*
        try{
                    SmsManager smsManager = SmsManager.getDefault();
                    smsManager.sendTextMessage(phone, null, msg, null, null);
                }catch (Exception e){
                    e.printStackTrace();
                }
            }
        });
    }

    public void onRequestPermissionResult(int requestCode, @NonNull String[] permission, @NonNull int[] grantResults){
        switch (requestCode){
            case MY_PERMISSION_REQUEST_SMS:{
                if(grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED){
                    Toast.makeText(this, "granted", Toast.LENGTH_SHORT).show();
                }else{
                    Toast.makeText(this, "denied", Toast.LENGTH_SHORT).show();
                }
            }
        }
    }
}*/
