import React, { useContext, useState } from 'react';
import { IonContent, IonInput, IonButton, IonItem, IonLabel, IonTextarea, IonSelect, IonSelectOption } from '@ionic/react';
import axios from 'axios';
import ApiWrapper from './APIWrapper';

const CreateVaultContainer: React.FC = () => {
  const [name, setName] = useState('');
  const [description, setDescription] = useState('');
  const [formValid, setFormValid] = useState(false);

  const handleSubmit = async (event: React.FormEvent<HTMLFormElement>) => {
    event.preventDefault();
    const formData = new URLSearchParams();
    formData.append('name', name);
    formData.append('description', description);

    await axios.post(ApiWrapper.backendURI + "vault/create", formData, {
      withCredentials: true, 
      headers: {
      'Content-Type': 'application/x-www-form-urlencoded'
      }
    })
    .then(response => console.log(response))
    .catch(error => console.log(error))

    console.log('Submitted:', { name, description });

  };

  const validateForm = () => {
    setFormValid(name.trim().length > 0);
  };

  return (
      <form onSubmit={handleSubmit}> 
          <IonItem>
            <IonLabel position="floating">Name</IonLabel>
            <IonInput
              type="text"
              value={name}
              onIonChange={(e) => {
                setName(e.detail.value!);
                validateForm();
              }}
            />
          </IonItem>

          <IonItem>
            <IonLabel position="floating">Description</IonLabel>
            <IonTextarea
              value={description}
              onIonChange={(e) => {
                setDescription(e.detail.value!);
                validateForm();
              }}
            />
          </IonItem>
          
          <IonButton expand="block" type="submit" disabled={!formValid}>Create</IonButton>
      </form>
  );
};

export default CreateVaultContainer;
