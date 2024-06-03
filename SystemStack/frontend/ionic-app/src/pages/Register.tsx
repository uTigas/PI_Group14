import React, { useState } from 'react';
import { IonPage, IonHeader, IonToolbar, IonTitle, IonContent, IonInput, IonButton, IonItem, IonLabel, IonIcon, IonCard, IonCardHeader, IonCardTitle, IonCardSubtitle, IonCardContent, IonList } from '@ionic/react';
import { personOutline, mailOutline, callOutline, textOutline } from 'ionicons/icons';
import ApiWrapper from '../support/APIWrapper';

const RegisterPage = () => {
  const [username, setUsername] = useState('');
  const [firstName, setFirstName] = useState('');
  const [lastName, setLastName] = useState('');
  const [email, setEmail] = useState('');
  const [phone, setPhone] = useState('');
  const [errors, setErrors] = useState<string[]>([]);
  
  const handleRegister = () => {
    const form = new FormData

    console.log('Registering user with:', { username, firstName, lastName, email, phone });
    const newErrors = []
    // Username validation
    if (username.trim().length < 1) {
      newErrors.push('Username is required');
    }
    form.append("username", username.trim())
    // First name validation
    if (firstName.trim().length < 1) {
      newErrors.push('First name is required');
    }
    form.append("first_name", firstName.trim())

    // Last name validation
    if (lastName.trim().length < 1) {
      newErrors.push('Last name is required');
    }
    form.append("last_name", lastName.trim())

    // Email validation
    const emailPattern = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    if (!emailPattern.test(email)) {
      newErrors.push('Invalid email address');
    }
    form.append("email", email.trim())

    // Phone validation
    const phonePattern = /^\d{9}$/;
    if (!phonePattern.test(phone)) {
      newErrors.push('Invalid phone number');
    }
    form.append("phone", phone.trim())

    setErrors(newErrors)
    if (newErrors.length == 0){
      //TODO: Procede to Registration in QKD
      ApiWrapper.register(form);
      //Register in Server

    }
  };

  return (
    <IonPage>
      <IonHeader translucent>
        <IonToolbar>
          <IonTitle>Register</IonTitle>
        </IonToolbar>
      </IonHeader>
      <IonContent fullscreen className="ion-padding">
      <IonCard className='ion-text-center ion-padding'>
        <IonCardHeader >
          <IonCardTitle>Welcome to Qeep!</IonCardTitle>
          <IonCardSubtitle><strong>Where your security problems come to an end.</strong> </IonCardSubtitle>
        </IonCardHeader>
        <IonCardContent>
          <IonItem lines="none">
            <IonIcon icon={personOutline} slot="start" />
            <IonInput placeholder="Username" value={username} onIonChange={e => setUsername(e.detail.value!)} />
          </IonItem>
          <IonItem lines="none">
            <IonIcon icon={textOutline} slot="start" />
            <IonInput placeholder="First Name" value={firstName} onIonChange={e => setFirstName(e.detail.value!)} />
          </IonItem>
          <IonItem lines="none">
            <IonIcon icon={textOutline} slot="start" />
            <IonInput placeholder="Last Name" value={lastName} onIonChange={e => setLastName(e.detail.value!)} />
          </IonItem>
          <IonItem lines="none">
            <IonIcon icon={mailOutline} slot="start" />
            <IonInput type="email" placeholder="E-mail" value={email} onIonChange={e => setEmail(e.detail.value!)} />
          </IonItem>
          <IonItem lines="none">
            <IonIcon icon={callOutline} slot="start" />
            <IonInput type="tel" placeholder="Phone" value={phone} onIonChange={e => setPhone(e.detail.value!)} />
          </IonItem>
          <IonButton expand="full" onClick={handleRegister} color="success">REGISTER</IonButton>
          
        </IonCardContent>
         {errors.length > 0 && (
            <div>
              <strong>Please fix the following errors:</strong>
              <IonList >
                {errors.map((error, index) => (
                  <IonItem  key={index} className='ion-text-center'>
                    <IonLabel color={'danger'}>{error}</IonLabel>
                  </IonItem>
                ))}
              </IonList>
            </div>
          )}
      </IonCard>
      </IonContent>
    </IonPage>
  );
};

export default RegisterPage;

