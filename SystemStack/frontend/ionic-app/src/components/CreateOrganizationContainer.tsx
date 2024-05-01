import React, { useState } from 'react';
import { IonContent, IonInput, IonButton, IonItem, IonLabel, IonTextarea, IonSelect, IonSelectOption } from '@ionic/react';

const CreateOrganizationContainer: React.FC = () => {
  const [organizationName, setOrganizationName] = useState('');
  const [description, setDescription] = useState('');
  const [size, setSize] = useState('');

  const [formValid, setFormValid] = useState(false);

  const handleSubmit = (event: React.FormEvent<HTMLFormElement>) => {
    event.preventDefault();
    // Here you can handle form submission, e.g., send data to backend
    console.log('Submitted:', { organizationName, description, size });
  };

  // Custom validation logic
  const validateForm = () => {
    setFormValid(organizationName.trim().length > 0 && description.trim().length > 0 && size.trim().length > 0);
  };

  return (
      <form onSubmit={handleSubmit}> 
          <IonItem>
            <IonLabel position="floating">Organization Name</IonLabel>
            <IonInput
              type="text"
              value={organizationName}
              onIonChange={(e) => {
                setOrganizationName(e.detail.value!);
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

          <IonItem>
            <IonLabel position="stacked">Size</IonLabel> {/* Use position="stacked" for labels above inputs */}
            <IonSelect
                label='Select Size'
                value={size}
                onIonChange={(e) => {
                setSize(e.detail.value!);
                validateForm();
                }}
                placeholder="Select Size"
            >
                <IonSelectOption value="small">Small</IonSelectOption>
                <IonSelectOption value="medium">Medium</IonSelectOption>
                <IonSelectOption value="large">Large</IonSelectOption>
            </IonSelect>
            </IonItem>

          <IonButton expand="block" type="submit" disabled={!formValid}>Create</IonButton>
      </form>
  );
};

export default CreateOrganizationContainer;
