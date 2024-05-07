import { IonButton, IonCol, IonContent, IonFooter, IonGrid, IonHeader, IonInput, IonItem, IonList, IonMenu, IonPage, IonRoute, IonRow, IonSplitPane, IonTitle, IonToolbar } from '@ionic/react';
import ExploreContainer from '../components/ExploreContainer';
import AppAppBar from '../components/AppAppBar';

import './Chats.css';

import firebase from 'firebase/compat/app';
import 'firebase/firestore';
import 'firebase/auth';

import { useAuthState, useSignInWithGoogle } from 'react-firebase-hooks/auth';
import { useCollectionData } from 'react-firebase-hooks/firestore';
import { GoogleAuthProvider, getAuth, signInWithPopup } from "firebase/auth"; // New import
import { addDoc, collection, getFirestore, limit, orderBy, query, serverTimestamp, setDoc } from 'firebase/firestore';
import { useRef, useState } from 'react';

/*
  TODOs(@cobileacd):
  - Fix styling
  - Replace tags with ionic specific
  - Get auth from main app 
  - Replace firebase with supabase(?)
  - Multiple chats support
  - ...
*/


firebase.initializeApp({
  apiKey: "AIzaSyCEehOj0G0N2kNwPiZcydqGuU2bVwMA8Eo",
  authDomain: "qubechat-c7d77.firebaseapp.com",
  projectId: "qubechat-c7d77",
  storageBucket: "qubechat-c7d77.appspot.com",
  messagingSenderId: "195600294469",
  appId: "1:195600294469:web:3943a1bece516b5a50792c",
  measurementId: "G-N0MJT80RP7"
})

export const auth = getAuth();
const firestore = getFirestore();

const Chats: React.FC = () => {

  const [user] = useAuthState(auth);

  return (
    <IonPage>
      <IonHeader>
        <AppAppBar title='Chats' />
      </IonHeader>
      <IonContent>
        <SignOut />
        {user ? <ChatRoom /> : <SignIn />}
      </IonContent>
    </IonPage>
  );
};

function SignIn() {
  const signInWithGoogle = () => {
    const provider = new GoogleAuthProvider();
    signInWithPopup(auth, provider);
  };

  return (
    <>
      <button className="sign-in" onClick={signInWithGoogle}>Sign in with Google</button>
    </>
  );
}

function SignOut() {
  return auth.currentUser && (
    <button onClick={() => auth.signOut()}>Sign Out</button>
  )
}

function ChatRoom() {
  const dummy = useRef();
  const messagesRef = collection(firestore, 'messages');
  const q = query(messagesRef, orderBy('createdAt'), limit(25));

  const [messages] = useCollectionData(q, { idField: 'id' });

  const [formValue, setFormValue] = useState('');


  const sendMessage = async (e: { preventDefault: () => void; }) => {
    if (formValue == '') { return; }
    e.preventDefault();

    const { uid, photoURL } = auth.currentUser;

    await addDoc(messagesRef, {
      text: formValue,
      createdAt: serverTimestamp(),
      uid,
      photoURL
    })

    setFormValue('');
    dummy.current.scrollIntoView({ behavior: 'smooth' });
  }

  return (
    <IonSplitPane when='xs' contentId='main'>
      <IonMenu contentId='main'>
        <IonHeader>
          <IonToolbar color='tertiary'>
            <IonTitle>Contacts</IonTitle>
          </IonToolbar>
        </IonHeader>
      </IonMenu>
      <div className='ion-page' id='main'>
        <IonHeader>
          <IonToolbar>
            <IonTitle>
              General
            </IonTitle>
          </IonToolbar>
        </IonHeader>
        <IonContent className='ion-padding'>
          <div className='Chat'>

              {messages && messages.map(msg => <ChatMessage key={msg.id} message={msg} />)}

              <span ref={dummy}></span>


            {/* 
            <form onSubmit={sendMessage}>

              <input value={formValue} onChange={(e) => setFormValue(e.target.value)} placeholder="say something nice" />

              <button type="submit" disabled={!formValue}>🕊️</button>

            </form>

            <IonList>
              <IonItem>
                <IonInput aria-label="text" placeholder='Enter text'></IonInput>
                <IonButton onClick={sendMessage} onChange={(e) => setFormValue(e.target.value)}>Send</IonButton>
              </IonItem>
            </IonList>
            */ }
          </div>
        </IonContent>
        <IonFooter>
          <IonToolbar>
            <IonGrid>
              <IonRow>
                <IonCol>
                  <IonInput aria-label="text" onIonInput={(e) => setFormValue(e.target.value)} placeholder='Enter text' value={formValue}></IonInput>
                </IonCol>
                <IonCol size='auto'  className='ion-text-end'>
                  <IonButton fill='outline' onClick={sendMessage} >Send</IonButton>
                </IonCol>
              </IonRow>
            </IonGrid>
          </IonToolbar>
        </IonFooter>
      </div>
    </IonSplitPane>)
}


function ChatMessage(props: { message: { text: any; uid: any; photoURL: any; }; }) {
  const { text, uid, photoURL } = props.message;

  const messageClass = uid === auth.currentUser.uid ? 'sent' : 'received';

  return (
    <>
      <div className={`message ${messageClass}`}>
        <img src={photoURL || 'https://api.adorable.io/avatars/23/abott@adorable.png'} />
        <p>{text}</p>
      </div>
    </>)
}

export default Chats;
